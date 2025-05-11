from pathlib import Path
from typing import Any, TypedDict, TypeVar, cast
from uuid import UUID

from django.contrib.auth import get_user_model
from django.contrib.auth.models import AbstractBaseUser
from django.contrib.contenttypes.models import ContentType
from django.db import connection
from django.db.models import F, Model, TextChoices, Value
from django.db.models.query import QuerySet
from django.utils.translation import gettext as _
from django.utils.translation import pgettext_lazy as _p
from rio.core.access.models import LockedObject

# Используется, чтобы этот модуль для тестирования можно было легко перенести
# в любой другой проект Django.
# В других случаях лучше импортировать rio.core.users.models.User явно.
User = get_user_model()

M = TypeVar("M", bound=Model)

class PermissionLabel(TextChoices):
    """
    Все доступные наименования разрешений.

    Все доступные разрешения обязательно указывать в этом перечислении.
    Хранить их в БД не имеет смысла: если разрешение не упоминается в коде,
    оно нигде не проверяется.
    """
    VIEW = "view", _p("Permission labels", "view")
    ADD = "add", _p("Permission labels", "add")
    CHANGE = "change", _p("Permission labels", "change")
    DELETE = "delete", _p("Permission labels", "delete")

class PermissionDict(TypedDict):
    user_id: UUID
    object_id: UUID
    permission: PermissionLabel
    reason: str | None

def qs_to_str(qs: QuerySet) -> str:
    return connection.cursor().mogrify(*qs.query.sql_with_params()).decode()

def all_ids(model: type[M]) -> QuerySet[M, dict[str, Any]]:
    return model._base_manager.all().order_by().annotate(object_id=F("pk")).values("object_id")

# user : User, object: M, permission: str, reason: str | None
def all_users_sql(
    model: type[M],
    permission: PermissionLabel,
    reason: str | None = None,
    *,
    allow_anonymous: bool,
    user_ids_qs: QuerySet[AbstractBaseUser, dict[str, Any]] | None = None,
    object_ids_qs: QuerySet[M, dict[str, Any]] | None = None,
) -> str:
    """
    Конструктор запроса декартова произведения
    всех идентификаторов пользователей
    и всех идентификаторов объектов, за исключением заблокированных,
    для режима доступа «для всех зарегистрированных» или «для всех пользователей»

    Параметры:

    * permission — строка, имя разрешения
    * reason — строка, причина разрешения
    * object_ids_query — queryset, получающий идентификаторы всех объектов с именем object_id
    * user_ids_query — queryset, получающий идентификаторы всех пользователей с именем user_id
    * allow_anonymous — boolean, true если к списку ID пользователей нужно добавить NULL,
      обозначающий незарегистрированного пользователя.
    """
    if object_ids_qs is None:
        object_ids_qs = all_ids(model, permission)
    if user_ids_qs is None:
        user_ids_qs = User._base_manager.order_by().annotate(user_id=F("pk")).values("user_id")
    template = """
    select
        objects.object_id as object_id,
        users.user_id as user_id,
        '{permission}' as permission,
        '{reason}' as reason
    from
        ({object_ids_query}) as objects,
        (
            {user_ids_query}
            union all
            select null as user_id
            where {allow_anonymous}
        ) as users
    """
    return template.format(
        permission=permission,
        reason=reason,
        object_ids_query=qs_to_str(object_ids_qs),
        user_ids_query=qs_to_str(user_ids_qs),
        allow_anonymous=allow_anonymous,
    )


def direct_chain(model: type[M], user_path: str, permission: PermissionLabel, reason: str) -> QuerySet[M, PermissionDict]:
    return cast(
        QuerySet[M, PermissionDict],
        model._base_manager.all().order_by()
        .filter(**{f"{user_path}__isnull": False})
        .annotate(object_id=F("pk"), user_id=F(user_path), permission=Value(permission), reason=Value(reason))
        .values("object_id", "user_id", "permission", "reason")
    )


def permission_chain(model: type[M], permission_object_path: str) -> QuerySet[M, PermissionDict]:
    return cast(
        "QuerySet[M, PermissionDict]",
        model._base_manager.all().order_by().filter(
            **{f"{permission_object_path}__isnull": False}  # Для inner join
        ).annotate(
            object_id=F("pk"),
            user_id=F(f"{permission_object_path}__user_id"),
            permission=F(f"{permission_object_path}__permission"),
            reason=F(f"{permission_object_path}__reason")
        ).values("object_id", "user_id", "permission", "reason")
    )

def get_permission_query(model: type[M], department_qs: str | QuerySet, **kwargs: QuerySet | str):
    """
    department_qs: путь к подразделениям, ответственные за которые должны иметь доступ к объекту.
    Запрос с полями ("pk", "department") или строка.
    Схема запроса:
        (разрешения по умолчанию там где нет LockedObject
        + разрешения из аргументов там где нет LockedObject)

        + dacpermission для всех разрешений
        - dacreject для всех разрешений
        + superuser для всех разрешений
    """
    if PermissionLabel.VIEW not in kwargs:
        kwargs[PermissionLabel.VIEW] = "SELECT * FROM ALL_USERS UNION ALL SELECT "

    if PermissionLabel.ADD not in kwargs:
        default_add = all_users_sql(
            model=model, permission=PermissionLabel.ADD, allow_anonymous=False, reason=_("Open for authenticated users")
        )
        kwargs[PermissionLabel.ADD] = default_add

    all_permissions = "\nUNION ALL\n".join(f"SELECT '{x}'" for x in kwargs.keys())
    rules = "\nUNION ALL\n".join(
        qs_to_str(q) if isinstance(q, QuerySet) else str(q) for q in kwargs.values()
    )

    if isinstance(department_qs, str):
        department_qs = all_ids(model).annotate(
            department=F(department_qs)
        ).values('object_id', 'department_id')

    path = Path(__file__).parent / "db_views" / "check_access.sql"
    with path.open() as fp:
        template = fp.read()

    return template.format(
        all_permissions=all_permissions,
        all_ids=qs_to_str(all_ids(model)),
        object_departments=qs_to_str(department_qs),
        rules=rules,
        reason_superuser=_("Superuser"),
        app_label=model._meta.app_label,
        model_name=model.__class__.__name__
    )