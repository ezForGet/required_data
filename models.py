from django.contrib.contenttypes.models import ContentType
from django.db import models
from django.utils.translation import gettext_lazy as _

from rio.common.models import RioModel
from rio.core.access.utils import PermissionLabel
from rio.core.departments.models import Department
from rio.core.users.models import User


class Role(RioModel):
    name = models.TextField(_("Role name"), null=False, blank=False)
    description = models.TextField(_("Role description"), null=False, blank=True)

class RolePermission(RioModel):
    role = models.ForeignKey(
        to=Role,
        on_delete=models.CASCADE,
    )
    content_type = models.ForeignKey(
        to=ContentType,
        verbose_name=_("Object type"),
        on_delete=models.PROTECT,
    )
    permission = models.CharField(
        verbose_name=_("Permission label"),
        max_length=64,
        null=False, blank=False, choices=PermissionLabel
    )

    class Meta:
        default_related_name = "role_permissions"


class UserRole(RioModel):
    user = models.ForeignKey(
        to=User,
        on_delete=models.CASCADE,
    )
    role = models.ForeignKey(
        to=Role,
        on_delete=models.CASCADE,
    )
    department = models.ForeignKey(
        to=Department,
        on_delete=models.CASCADE,
    )
    reason = models.TextField()


class DACPermission(RioModel):
    user = models.ForeignKey(
        to=User,
        on_delete=models.CASCADE,
    )
    content_type = models.ForeignKey(
        to=ContentType,
        verbose_name=_("Object type"),
        on_delete=models.PROTECT,
    )
    # Что делать, если мы хотим выдавать разрешения на другие модели, у которых id не UUIDField?
    object_id = models.UUIDField()
    permission = models.CharField(
        verbose_name=_("Permission label"),
        max_length=64,
        null=False, blank=False, choices=PermissionLabel
    )
    reason = models.TextField()


class DACRestriction(RioModel):
    user = models.ForeignKey(
        to=User,
        on_delete=models.CASCADE,
    )
    content_type = models.ForeignKey(
        to=ContentType,
        verbose_name=_("Object type"),
        on_delete=models.PROTECT,
    )
    # Что делать, если мы хотим выдавать ограничения на другие модели?
    object_id = models.UUIDField()
    permission = models.CharField(
        verbose_name=_("Permission label"),
        max_length=64,
        null=False, blank=False, choices=PermissionLabel
    )
    reason = models.TextField()


class LockedObject(RioModel):
    content_type = models.ForeignKey(
        to=ContentType,
        verbose_name=_("Object type"),
        on_delete=models.PROTECT,
    )
    # Что делать, если мы хотим выдавать блокировки на другие модели?
    object_id = models.UUIDField()
    permission = models.CharField(
        verbose_name=_("Permission label"),
        max_length=64,
        null=False, blank=False, choices=PermissionLabel
    )
    department = models.ForeignKey(
        to=Department,
        on_delete=models.CASCADE,
    )
    reason = models.TextField()