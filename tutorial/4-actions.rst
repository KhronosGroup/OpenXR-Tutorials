4 Interactions
=======

4. ACTIONS (cube at controller position at end)

Using Spaces
------------

4.1. Using Spaces ( xrCreateReferenceSpace/xrLocateSpace ) (draw cube at world
origin, view from VIEW space)


OpenXR uses a couple different types of reference frames for positioning content.
STAGE would be relative to the center of your guardian system's bounds, and LOCAL
would be relative to your device's starting location. HoloLens doesn't have a STAGE, so we'll use LOCAL.

https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrReferenceSpaceType.html


The Interaction System
----------------------

4.2. The interaction system. Interaction Profiles, Bindings best practices

Creating Actions and Action Sets
--------------------------------

4.3. Create Actions and Actions Sets (xrCreateActionSet, xrCreateAction,
xrSuggestInteractionProfileBindings)

Checking for Connected Controllers
----------------------------------

4.4. How to check if a controller is connected.
