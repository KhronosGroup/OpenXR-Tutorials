4 Interactions
==============

An OpenXR application has interactions with the user. These interactions can be user input to the application, or haptic output back to the user. In this chapter, we will create some
interactions and show how this system works.

4.1. Using Spaces
-----------------

Key to the interaction system is the concept of a *space*. A space is a frame of reference
defined not by its instantaneous values, but semantically, by its purpose and relationship to other spaces. For the actual, instantaneous position and orientation of a space, we call this a "pose".

In Section 3.2, we created a _reference_ space called ```localOrStageSpace``` which represents the stationary space within which the application runs. We needed this space to call ```xrLocateViews``` because this function generates a view matrix for each eye. Recall that a view matrix transforms from a "global" reference frame into one oriented with the eye or camera.

By using a local space - defined with ```XR_REFERENCE_SPACE_TYPE_LOCAL``` - we specify that the views are relative to the XR hardware's "local" space - either the headset's starting position or some other world-locked origin.

Some devices support stage space - ```XR_REFERENCE_SPACE_TYPE_STAGE``` - this implies a roomscale space with its origin on the floor.

How these are actually interpreted is a matter for the OpenXR runtime.

When we created the reference space, we specified a pose (```poseInReferenceSpace```) of identity:

    referenceSpaceCI.poseInReferenceSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

i.e. an identity quaternion for the orientation, a position at the origin.

We had no reason to want a different origin for our space than the runtime's default. Had we specified a different pose, the origin of the reference space would have been offset from that default.

Another kind of reference space is view space (```XR_REFERENCE_SPACE_TYPE_VIEW```).
View space is oriented with the user's head, and is useful for user-interface and many
other purposes. We don't use it to generate view matrices for rendering, because those are often offset from the view space due to stereo rendering.

OpenXR uses a couple of different types of reference frames for positioning content.
STAGE would be relative to the center of your guardian system's bounds, and LOCAL
would be relative to your device's starting location. HoloLens doesn't have a STAGE, so we'll use LOCAL.

https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrReferenceSpaceType.html


4.2. The Interaction System
---------------------------

Interaction Profiles, Bindings best practices.
(Not clear this is the best place for this section.)



4.3. Creating Actions and Action Sets
--------------------------------

At the end of your application class, add this code:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Actions
	:end-before: XR_DOCS_TAG_END_Actions
	:dedent: 1

Here, we have an Action Set: a group of related actions that are created together. The individual actions, such as Select, Trigger, LeftGripPose, will belong to this set.
For a pose action, we need an XrSpace, like xrSpaceLeftGripPose. And we'll keep a copy of the pose itself, which will change per-frame.

4.3. Create Actions and Actions Sets (xrCreateActionSet, xrCreateAction,
xrSuggestInteractionProfileBindings)

ActionSet: A group of related actions for a given context, environment etc.
Action: A semantic interaction

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_InteractionDefinitions
	:end-before: XR_DOCS_TAG_END_InteractionDefinitions
	:dedent: 1


Rendering the Controller position
---------------------------------

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_include_linear_algebra
	:end-before: XR_DOCS_TAG_END_include_linear_algebra
	:dedent: 0


Checking for Connected Controllers
----------------------------------

4.4. How to check if a controller is connected.
