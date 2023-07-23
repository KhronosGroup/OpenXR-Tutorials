🕹 4 Interactions
==================

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


4.2 Interaction Profiles and Bindings
-------------------------------------

As OpenXR is an API for many different devices, it needs to provide a way for you as a developer to refer to the various buttons, joysticks, inputs and outputs that a device may have, without needing to know in advance which device or devices the user will have.

To do this, OpenXR defines the concept of *interaction profiles*. An interaction profile is a collection of interactions supported by the given device and runtime, and defined by means of textual paths.

The first element of the interaction profile is the Profile Path, of the form "/interaction_profiles/<vendor_name>/<type_name>". Then, each interaction is defined by a
path with three elements: the Profile Path, the User Path, and the Component Path. The User Path is a string identifying the controller device, e.g. "/user/hand/left", or "/user/gamepad". The Component Path is a string identifying the specific input or output, e.g. "/input/select/click" or "/output/haptic_left_trigger".

For example, the Khronos Simple Controller Profile, supported by many devices, has the path "/interaction_profiles/khr/simple_controller", and user paths "/user/hand/left" and "/user/hand/right". The component paths are "/input/select/click", "/input/menu/click", "/input/grip/pose", "/input/aim/pose", "/output/haptic". Putting the three parts together, we might identify the select button on the left hand controller as "/interaction_profiles/khr/simple_controller/user/hand/left/input/select/click".

Each device can support any number of interaction profiles, either the nine profiles defined in the OpenXR standard, or an extension profile (see https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#_adding_input_sources_via_extensions).

See also https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#semantic-path-interaction-profiles.

We will see in Section 4.4 how  to suggest a binding between interaction profile paths and application inputs and outputs. Firt, we will learn how to define these inputs and outputs, which in OpenXR, we call *actions*.

4.3 Creating Actions and Action Sets
------------------------------------

At the end of your application class, add this code:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Actions
	:end-before: XR_DOCS_TAG_END_Actions
	:dedent: 1

Here, we have defined an Action Set: a group of related actions that are created together. The individual actions, such as selectAction and triggerAction, will belong to this set. For a pose action, we need an XrSpace, so leftGripPoseSpace has been declared. And we'll keep a copy of the pose itself, leftGripPose, which will change per-frame.


ActionSets are created before the session is initialized, so in Run(), after the call to GetSystemID(), add this line:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateActionSet
	:end-before: XR_DOCS_TAG_END_CallCreateActionSet
	:dedent: 1

Now we will define the function: insert this after the definition of GetSystemID():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActionSet
	:end-before: XR_DOCS_TAG_END_CreateActionSet
	:dedent: 1

You can create multiple ActionSets, but we only need one for this example. The ActionSet is created with a name, and a localized string for its description. Now add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActions
	:end-before: XR_DOCS_TAG_END_CreateActions
	:dedent: 1

Here we've created each action with a little local lambda function `CreateAction`. Each action has a name, a localized description, and the type of action it is.

4.4 Binding Interactions
------------------------

We will set up bindings for the actions. A binding is a *suggested* correspondence between an action (which is app-defined), and the input/output on the
user's devices. After the definition of CreateActionSet(), we'll add this helper function that converts a string into an XrPath. Add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateXrPath
	:end-before: XR_DOCS_TAG_END_CreateXrPath
	:dedent: 1

XrPath is a 64-bit number that hopefully uniquely identifies any given forward-slash-delimited path string, allowing us to refer to paths without putting cumbersome string-handling in our runtime code. After the call to CreateActionSet() in Run(), add the line:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallSuggestBindings
	:end-before: XR_DOCS_TAG_END_CallSuggestBindings
	:dedent: 1

After the definition of CreateActionSet(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SuggestBindings
	:end-before: XR_DOCS_TAG_END_SuggestBindings
	:dedent: 1

Here, we create a *suggested* match-up between our app-specific actions, and XrPaths which refer to specific controls as interpreted by the Runtime. We call `xrSuggestInteractionProfileBindings()`. If the user's device supports the given profile "/interaction_profiles/khr/simple_controller", it will recognize these paths and can map them to its own controls. If the user's device does not support this profile, the bindings will be ignored.

**Bindings best practices**

The suggested bindings are not guaranteed to be used: that's up to the runtime. Some runtimes allow users to override the default bindings, and OpenXR expects this.

ActionSets and Suggested Bindings are created before the session is initialized. There is session-specific setup to be done for our actions also. After the call to CreateResources(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateActionPoses
	:end-before: XR_DOCS_TAG_END_CallCreateActionPoses
	:dedent: 2

Now after the definition of SuggestBindings(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActionPoses
	:end-before: XR_DOCS_TAG_END_CreateActionPoses
	:dedent: 1

For one pose, this didn't need to be a lambda, but it will make it easier to add more poses later. Here, we're creating the XrSpace that represents the left grip pose action. As with the reference space, we use an identity XrPosef to indicate that we'll take the pose as-is, without offsets.

Finally as far as action setup goes, we will attach the ActionSet to the session. Add this function:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_AttachActionSet
	:end-before: XR_DOCS_TAG_END_AttachActionSet
	:dedent: 1

As you can see, it's possible here to attach multiple Action Sets. But `xrAttachSessionActionSets` can only be called *once* per session. You have to know what Action Sets you will be using before the session can start - xrBeginSession() is called from PollEvents() once all setup is complete and the app is ready to proceed.

Now, we must poll the actions, once per-frame. Add this call in RenderFrame() just before the call to RenderLayer():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallPollActions
	:end-before: XR_DOCS_TAG_END_CallPollActions
	:dedent: 3

And add this function after the definition of PollEvents():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions
	:end-before: XR_DOCS_TAG_END_PollActions
	:dedent: 1

Here we enable the Action Set we're interested in (in our case we have only one), and tell OpenXR to prepare the actions' per-frame data with xrSyncActions().

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions2
	:end-before: XR_DOCS_TAG_END_PollActions2
	:dedent: 1

This is the polling code. We specify which action to look at with the XrActionStateGetInfo struct. Then we use a type-specific call. For our boolean `selectAction`, we call `xrGetActionStateBoolean()` to retrieve an XrActionStateBoolean struct. This specifies whether the value of the boolean is true or false, and we can use this to determine whether the user is pressing the select button on the controller. But there's more to it than that. The struct `XrActionStateBoolean` has a boolean `isActive`, which is true if the state of the action is actually being read. If `isActive` is false, the value of `currentState` is irrelevant - the polling failed. The struct has `changedSinceLastSync`, which is true if the value changed between the previous and current calls to xrSync(). And it has `lastChangeTime`, which is the time at which the value last changed. This allows us to be very precise about when the user pressed the button, and how long they held it down for. This could be used to detect "long presses", or double-clicks.

Similarly, `XrActionStateFloat` has a floating-point `currentState` value, which is valid if `isActive` is true, it has `lastChangeTime` and `changedSinceLastSync`, and it has `isActive` and `lastChangeTime` and `changedSinceLastSync`.

Careful use of this polling metadata will help you to create apps that are responsive and intuitive to use. Bear in mind as well that multiple physical controls could be bound to the same action, and the user could be using more than one controller at once. See the OpenXR spec for more details:

https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#multiple_inputs

Finally in this function, we'll poll the left Grip Pose:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions3
	:end-before: XR_DOCS_TAG_END_PollActions3
	:dedent: 1

If, and only if the action is active, we use `xrLocateSpace` to obtain the current pose of the controller. We specify that we want this relative to our reference space `localOrStageSpace`, because this is the global space we're using for rendering. We'll use `leftGripPose` in the next section to render the
controller's position.

4.5 Rendering the Controller position
-------------------------------------

We will now draw some geometry to represent the controller pose we've obtained as `leftGripPose`. Add this function after the definition of RenderLayer():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_include_linear_algebra
	:end-before: XR_DOCS_TAG_END_include_linear_algebra
	:dedent: 0

This provides a simple matrix and vector library for our render code.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateResources1
	:end-before: XR_DOCS_TAG_END_CreateResources1
	:dedent: 0
	
.. container:: opengl vulkan

	.. literalinclude:: ../Chapter4/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_OpenGL_Vulkan
		:end-before: XR_DOCS_TAG_END_CreateResources2_OpenGL_Vulkan
		:dedent: 0
		
.. container:: opengles

	.. literalinclude:: ../Chapter4/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_OpenGLES
		:end-before: XR_DOCS_TAG_END_CreateResources2_OpenGLES
		:dedent: 0
		
.. container:: d3d11 d3d12

	.. literalinclude:: ../Chapter4/main.cpp
		:language: cpp
		:start-after: XR_DOCS_TAG_BEGIN_CreateResources2_D3D
		:end-before: XR_DOCS_TAG_END_CreateResources2_D3D
		:dedent: 0
		

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateResources3
	:end-before: XR_DOCS_TAG_END_CreateResources3
	:dedent: 0

4.6 Checking for Connected Controllers
--------------------------------------

How to check if a controller is connected.
