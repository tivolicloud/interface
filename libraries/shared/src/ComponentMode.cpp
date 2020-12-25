//
//  Created by Sam Gondelman on 5/31/19
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "ComponentMode.h"

/**jsdoc
 * <p>How an effect is applied in a {@link Entities.EntityProperties-Zone|Zone} entity.</p>
 * <table>
 *   <thead>
 *     <tr><th>Value</th><th>Description</th></tr>
 *   </thead>
 *   <tbody>
 *     <tr><td><code>"inherit"</code></td><td>The effect from any enclosing zone continues into this zone.</td></tr>
 *     <tr><td><code>"disabled"</code></td><td>The effect &mdash; from any enclosing zone and this zone &mdash; is disabled in 
 *       this zone.</td></tr>
 *     <tr><td><code>"enabled"</code></td><td>The effect from this zone is enabled, overriding the effect from any enclosing 
 *       zone.</td></tr>
 *   </tbody>
 * </table>
 * @typedef {string} Entities.ComponentMode
 */
const char* componentModeNames[] = {
    "inherit",
    "disabled",
    "enabled"
};

QString ComponentModeHelpers::getNameForComponentMode(ComponentMode mode) {
    if (((int)mode <= 0) || ((int)mode >= (int)COMPONENT_MODE_ITEM_COUNT)) {
        mode = (ComponentMode)0;
    }

    return componentModeNames[(int)mode];
}


/**jsdoc
 * <p>How zone culling is applied in a {@link Entities.EntityProperties-Zone|Zone} entity.</p>
 * <table>
 *   <thead>
 *     <tr><th>Value</th><th>Description</th></tr>
 *   </thead>
 *   <tbody>
 *     <tr><td><code>"inherit"</code></td><td>Inherits whatever zone culling paroperties are outside this zone.</td></tr>
 *     <tr><td><code>"outside"</code></td><td>Culls everything outside of this + any outer zones.</td></tr>
 *     <tr><td><code>"disabled"</code></td><td>Culls nothing, including overriding outer zone culling..</td></tr>
 *   </tbody>
 * </table>
 * @typedef {string} Entities.ZoneCullingMode
 */
const char* zoneCullingModeNames[] = { "inherit", "outside", "disabled" };


QString ZoneCullingModeHelpers::getNameForZoneCullingComponentMode(ZoneCullingComponentMode mode) {
    if (((int)mode <= 0) || ((int)mode >= (int)ZONE_CULLING_MODE_ITEM_COUNT)) {
        mode = (ZoneCullingComponentMode)0;
    }

    return zoneCullingModeNames[(int)mode];
}

/**jsdoc
 * <p>How tone mapping is applied in a {@link Entities.EntityProperties-Zone|Zone} entity.</p>
 * <table>
 *   <thead>
 *     <tr><th>Value</th><th>Description</th></tr>
 *   </thead>
 *   <tbody>
 *     <tr><td><code>"inherit"</code></td><td>Inherits whatever tone mapping curve paroperties are outside this zone.</td></tr>
 *     <tr><td><code>"rgb"</code></td><td>Apply RGB tone mapping in the zone.</td></tr>
 *     <tr><td><code>"srgb"</code></td><td>Apply SRGB tone mapping in the zone.</td></tr>
 *     <tr><td><code>"reinhard"</code></td><td>Apply Reinhard tone mapping in the zone.</td></tr>
 *     <tr><td><code>"filmic"</code></td><td>Apply filmic tone mapping in the zone.</td></tr>
 *   </tbody>
 * </table>
 * @typedef {string} Entities.ToneMappingMode
 */

const char* ToneMappingModeNames[] = { "inherit", "rgb", "srgb", "reinhard", "filmic" };


QString ToneMappingModeHelpers::getNameForToneMappingComponentMode(ToneMappingComponentMode mode) {
    if (((int)mode <= 0) || ((int)mode >= (int)TONE_MAPPING_MODE_ITEM_COUNT)) {
        mode = (ToneMappingComponentMode)0;
    }

    return ToneMappingModeNames[(int)mode];
}

/**jsdoc
 * <p>The priority of updates from avatars in a zone to other clients.</p>
 * <table>
 *   <thead>
 *     <tr><th>Value</th><th>Description</th></tr>
 *   </thead>
 *   <tbody>
 *     <tr><td><code>"inherit"</code></td><td>The update priority from any enclosing zone continues into this zone.</td></tr>
 *     <tr><td><code>"crowd"</code></td><td>The update priority in this zone is the normal priority.</td></tr>
 *     <tr><td><code>"hero"</code></td><td>Avatars in this zone have an increased update priority.</td></tr>
 *   </tbody>
 * </table>
 * @typedef {string} Entities.AvatarPriorityMode
 */
const char* avatarPriorityModeNames[] = {
    "inherit",
    "crowd",
    "hero"
};

QString AvatarPriorityModeHelpers::getNameForAvatarPriorityMode(AvatarPriorityMode mode) {
    if (((int)mode <= 0) || ((int)mode >= (int)AVATAR_PRIORITY_ITEM_COUNT)) {
        mode = (AvatarPriorityMode)0;
    }

    return avatarPriorityModeNames[(int)mode];
}
