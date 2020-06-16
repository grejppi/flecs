
#ifndef FLECS_SYSTEMS_H
#define FLECS_SYSTEMS_H

#include "flecs.h"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
extern ecs_type_t
    ecs_type(EcsTrigger),
    ecs_type(EcsModule),
    ecs_type(EcsSystem),
    ecs_type(EcsTickSource),
    ecs_type(EcsSignatureExpr),
    ecs_type(EcsSignature),
    ecs_type(EcsQuery),
    ecs_type(EcsIterAction),
    ecs_type(EcsContext);

/* Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;           /* True if providing tick */
    float time_elapsed;  /* Time elapsed since last tick */
} EcsTickSource;

/* Signature expression */
typedef struct EcsSignatureExpr {
    const char *expr;
} EcsSignatureExpr;

/* Parsed signature */
typedef struct EcsSignature {
    ecs_sig_t signature;
} EcsSignature;

/* Query component */
typedef struct EcsQuery {
    ecs_query_t *query;
} EcsQuery;

/* Component used for registering component triggers */
typedef struct EcsTrigger {
    ecs_entity_t kind;
    ecs_iter_action_t action;
    ecs_entity_t component;
    ecs_entity_t self;
    void *ctx;
} EcsTrigger;

/* System action */
typedef struct EcsIterAction {
    ecs_iter_action_t action;
} EcsIterAction;

/* System context */
typedef struct EcsContext {
    const void *ctx;
} EcsContext;


////////////////////////////////////////////////////////////////////////////////
//// Systems API
////////////////////////////////////////////////////////////////////////////////

/** Run a specific system manually.
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * Manual systems are useful to evaluate lists of prematched entities at
 * application defined times. Because none of the matching logic is evaluated
 * before the system is invoked, manual systems are much more efficient than
 * manually obtaining a list of entities and retrieving their components.
 *
 * An application may pass custom data to a system through the param parameter.
 * This data can be accessed by the system through the param member in the
 * ecs_iter_t value that is passed to the system callback.
 *
 * Any system may interrupt execution by setting the interrupted_by member in
 * the ecs_iter_t value. This is particularly useful for manual systems, where
 * the value of interrupted_by is returned by this operation. This, in
 * cominbation with the param argument lets applications use manual systems
 * to lookup entities: once the entity has been found its handle is passed to
 * interrupted_by, which is then subsequently returned.
 *
 * @param world The world.
 * @param system The system to run.
 * @param delta_time: The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_EXPORT
ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param);

/** Run system with offset/limit and type filter.
 * This operation is the same as ecs_run, but filters the entities that will be
 * iterated by the system.
 * 
 * Entities can be filtered in two ways. Offset and limit control the range of
 * entities that is iterated over. The range is applied to all entities matched
 * with the system, thus may cover multiple archetypes.
 * 
 * The type filter controls which entity types the system will evaluate. Only
 * types that contain all components in the type filter will be iterated over. A
 * type filter is only evaluated once per table, which makes filtering cheap if
 * the number of entities is large and the number of tables is small, but not as
 * cheap as filtering in the system signature.
 * 
 * @param world The world.
 * @param system The system to invoke.
 * @param delta_time: The time passed since the last system invocation.
 * @param filter A component or type to filter matched entities.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_EXPORT
ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param);

/** System status change callback */
typedef enum ecs_system_status_t {
    EcsSystemStatusNone = 0,
    EcsSystemEnabled,
    EcsSystemDisabled,
    EcsSystemActivated,
    EcsSystemDeactivated
} ecs_system_status_t;

typedef void (*ecs_system_status_action_t)(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx);

/** Set system status action.
 * The status action is invoked whenever a system is enabled or disabled. Note
 * that a system may be enabled but may not actually match any entities. In this
 * case the system is enabled but not _active_.
 *
 * In addition to communicating the enabled / disabled status, the action also
 * communicates changes in the activation status of the system. A system becomes
 * active when it has one or more matching entities, and becomes inactive when
 * it no longer matches any entities.
 * 
 * A system switches between enabled and disabled when an application invokes the
 * ecs_enable operation with a state different from the state of the system, for
 * example the system is disabled, and ecs_enable is invoked with enabled: true.
 *
 * Additionally a system may switch between enabled and disabled when it is an
 * EcsOnDemand system, and interest is generated or lost for one of its [out]
 * columns.
 *
 * @param world The world.
 * @param system The system for which to set the action.
 * @param action The action.
 * @param ctx Context that will be passed to the action when invoked.
 */
FLECS_EXPORT
void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/* Pipeline component is empty: components and tags in module are static */
typedef struct FlecsSystems {
    int32_t dummy; 
} FlecsSystems;

FLECS_EXPORT
void FlecsSystemsImport(
    ecs_world_t *world,
    int flags);

#define FlecsSystemsImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif