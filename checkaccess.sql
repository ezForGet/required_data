-- Шаблон запроса для разграничения доступа
-- В качестве параметров принимает четыре запроса и три строки.
-- Запросы:
--  - all_permissions — список всех доступных разрешений для объекта
--  - all_ids — список всех идентификаторов объектов
--  - object_departments — список всех подразделений, ответственные по которым должны иметь доступ к объекту.
--    В этом запросе не нужно учитывать иерархию подразделений, она применяется далее в шаблоне.
--  - rules — запрос со стандартными правилами доступа
-- Строки:
--  - reason_superuser — значение reason для суперпользователя
--  - app_label, model_name — имя модели для получения ContentType

WITH
-- параметры
CONTENT_TYPE_ID(content_type_id) AS (
    SELECT id AS content_type_id
    FROM django_content_type
    WHERE app_label = '{app_label}' AND model = '{model}'
),
ALL_PERMISSIONS(permission) AS ({all_permissions}),
ALL_IDS(object_id) AS ({all_ids}),
OBJECT_DEPARTMENTS(object_id, department_id) AS ({object_departments}),
RULES AS ({rules}),

-- простые множества
LOCKED_OBJECTS(object_id, department, permission) AS (
    SELECT object_id, department, permission
    FROM access_lockedobject
    NATURAL JOIN CONTENT_TYPE_ID
),
ROLE_PERMISSIONS(user_id, permission, reason, department_id, authority_department_id) AS (
    SELECT
        user_id,
        permission,
        reason,
        departments_currentnesteddepartmentrelation.descendant_id as department_id
        access_userrole.department_id as authority_department_id
    FROM
        access_userrole
        JOIN access_rolepermission ON access_userrole.role_id = access_rolepermission.role_id
        JOIN departments_currentnesteddepartmentrelation ON
            access_userrole.department_id = departments_currentnesteddepartmentrelation.ancestor_id
),

-- cоставные множества
NORMAL_RULES(object_id, user_id, permission, reason) AS (
    SELECT * FROM RULES
    WHERE NOT EXISTS (
        SELECT object_id, permission
        FROM access_lockedobject NATURAL JOIN CONTENT_TYPE_ID
        WHERE (RULES.object_id, RULES.permission) = (access_lockedobject.object_id, access_lockedobject.permission)
    )
),
NORMAL_REPRESENTATIVE_RULES(object_id, user_id, permission, reason) AS (
    SELECT
        OBJECT_DEPARTMENTS.object_id AS object_id,
        ROLE_PERMISSIONS.user_id AS user_id,
        ROLE_PERMISSIONS.permission AS permission,
        ROLE_PERMISSIONS.reason AS REASON
    FROM
        OBJECT_DEPARTMENTS
        JOIN ROLE_PERMISSIONS ON OBJECT_DEPARTMENTS.department_id = ROLE_PERMISSIONS.department_id
    WHERE NOT EXISTS (
        SELECT object_id, permission
        FROM access_lockedobject NATURAL JOIN CONTENT_TYPE_ID
        WHERE (OBJECT_DEPARTMENTS.object_id, ROLE_PERMISSIONS.permission)
            = (access_lockedobject.object_id, access_lockedobject.permission)
    )
),
LOCKED_REPRESENTATIVE_RULES(object_id, user_id, permission, reason) AS (
    SELECT
        LOCKED_OBJECTS.object_id AS object_id,
        ROLE_PERMISSIONS.user_id AS user_id,
        LOCKED_OBJECTS.permission AS permission,
        ROLE_PERMISSIONS.reason AS reason
    FROM
        LOCKED_OBJECTS
        JOIN departments_currentnesteddepartmentrelation ON
            LOCKED_OBJECT.department_id = departments_currentnesteddepartmentrelation.descendant_id
        JOIN ROLE_PERMISSIONS ON (
            (LOCKED_OBJECTS.object_id, LOCKED_OBJECTS.permission, departments_currentnesteddepartmentrelation.ancestor_id)
            = (ROLE_PERMISSIONS.object_id, ROLE_PERMISSIONS.permission, ROLE_PERMISSIONS.authority_department_id)
),
DAC_PERMISSION_RULES(object_id, user_id, permission, reason) AS (
    SELECT object_id, user_id, permission, reason
    FROM access_dacpermission
    NATURAL JOIN CONTENT_TYPE_ID
),
DAC_RESTRICTION_RULES(object_id, user_id, permission) AS (
    SELECT object_id, user_id, permission
    FROM access_dacrestriction
    NATURAL JOIN CONTENT_TYPE_ID
),
SUPERUSER_RULES(object_id, user_id, permission, reason) AS (
    SELECT
        object_id,
        user_id,
        permission,
        '{reason_superuser}' AS reason
    FROM
        (SELECT id AS user_id FROM users_user WHERE is_superuser)
        CROSS JOIN ALL_IDS CROSS JOIN ALL_PERMISSIONS
)

SELECT * FROM (
    SELECT * FROM NORMAL_RULES
    UNION ALL
    SELECT * FROM NORMAL_REPRESENTATIVE_RULES
    UNION ALL
    SELECT * FROM LOCKED_REPRESENTATIVE_RULES
    UNION ALL
    SELECT * FROM DAC_PERMISSION_RULES
) AS UNRESTRICTED
WHERE NOT EXISTS (
    SELECT * FROM DAC_RESTRICTION_RULES
    WHERE (UNRESTRICTED.user_id, UNRESTRICTED.object_id, UNRESTRICTED.permission)
        = (DAC_RESTRICTION_RULES.user_id, DAC_RESTRICTION_RULES.object_id, DAC_RESTRICTION_RULES.permission)
)
UNION ALL
SELECT * FROM SUPERUSER_RULES