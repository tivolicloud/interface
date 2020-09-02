//
//  Created by Caitlyn Meeks
//  (c) 2020 Tivoli Cloud VR
//

#ifndef hifi_EntityPriority_h
#define hifi_EntityPriority_h

#include "QString"

/**jsdoc
 * <p>Sets the priority with which the entity receives updates from the CPU. <code>"static"</code> receives fewer updates for fast processing. <code>"automatic"</code> calculates optimal update rate for CPU performance, but may cause animations to stutter. <code>"prioritized"</code> provides smoothest updating but can tax the CPU, so use judiciously.</p>
 * <table>
 *   <thead>
 *     <tr><th>Value</th><th>Description</th></tr>
 *   </thead>
 *   <tbody>
 *     <tr><td><code>"automatic"</code></td><td>The entity receives periodic updates as needed.</td></tr>
 *     <tr><td><code>"static"</code></td><td>The entity does not need to receive updates, like scenery. Saves on CPU.</td></tr>
 *     <tr><td><code>"prioritized"</code></td><td>The entity receives frequent updates.  Expensive on CPU.</td></tr>
 *   </tbody>
 * </table>
 * @typedef {string} Entities.EntityPriority
 */

enum class EntityPriority {
    STATIC = 0,
    AUTOMATIC,
    PRIORITIZED
};

class EntityPriorityHelpers {
public:
    static QString getNameForEntityPriority(EntityPriority mode);
};

#endif // hifi_EntityPriority_h

