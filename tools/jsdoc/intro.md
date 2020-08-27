<h2 style="text-align: center; margin-top: -1em;">JavaScript API Reference</h2>
<h3 style="text-align: center; margin-top: 0; font-weight: 400;">Version {{ build_version }}</h3>

The Tivoli Cloud JavaScript API lets content creators and developers create new experiences and transform virtual worlds within the Tivoli Cloud metaverse. With it, you can build great content, customize avatars, play audio and so much more.

You are most likely to interact with these APIs:

-   The **[Entities](Entities.html)** namespace lets you add, remove, and edit entities around you to build an interactive environment. In addition, you can use this namespace to find entities in range, direction, collision, or raytrace.
-   The **[AvatarList](AvatarList.html)**, **[MyAvatar](MyAvatar.html)**, and **[Avatar](Avatar.html)** namespaces affect your personal avatars, and lets you get information on other people's avatars.
-   The **[Script](Script.html)** namespace lets you to connect callbacks from your client to script, such as functionality that is dependent on time ([`Script.update`](Script.html#.update), [`Script.setTimeout`](Script.html#.setTimeout), [`Script.setInterval`](Script.html#.setInterval), etc), connect paths relatively to assets ([`Script.resolvePath`](Script.html#.resolvePath)), refer to other scripts ([`Script.require`](Script.html#.require), [`Script.include`](Script.html#.include)), or connect functions to events which occur when the script is turned off ([`Script.scriptEnding`](Script.html#.scriptEnding)).
