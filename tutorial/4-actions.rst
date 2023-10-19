##############
4 Interactions
##############

The goal of this chapter is to build an application uses OpenXR actions to create interactivity. We will render numerous colored cubes for the user play with and build stuctures. First, we will update our build files and folders.

.. container:: windows linux

	In the *workspace* directory, update the ``CMakeLists.txt`` by adding the following CMake code to the end of the file:

	.. literalinclude:: ../CMakeLists.txt
			:language: cmake
			:start-after: XR_DOCS_TAG_BEGIN_AddChapter4
			:end-before: XR_DOCS_TAG_END_AddChapter4
			:dedent: 4

	Now, create a ``Chapter4`` folder in the *workspace* directory and into that folder copy the ``main.cpp`` from ``Chapter3``. For the ``CMakeLists.txt``, copy the from ``Chapter3`` and update these lines:

.. container:: android

	Add a ``Chapter4`` folder in the *workspace* directory and into that folder copy the whole contents of ``Chapter3`` to it. In CMakeLists.txt, update this line:

.. literalinclude:: ../Chapter4/CMakeLists.txt
	:language: cmake
	:start-after: XR_DOCS_TAG_BEGIN_SetProjectName4
	:end-before: XR_DOCS_TAG_END_SetProjectName4

.. container:: android

	For Android, you will also need to change all references to ``OpenXRTutorialChapter3`` to ``OpenXRTutorialChapter4`` in app/build.gradle, settings.gradle and app/src/main/AndroidManifest.xml. Refer to :ref:`Chapter 1.4.1<1.4.1 CMake and Project Files>` for a refresher on the Android build files.

	Load your new project in Android Studio.

Now build your project, you can run it to check that it behaves the same as your Chapter 3 code so far.

*********************************
4.1 The OpenXR Interaction System
*********************************

OpenXR interactions are defined in a two-way system with inputs such as motion-based poses, button presses, analogue controls, and outputs such as haptic controller vibrations. Collectively in OpenXR terminology, these are called Actions. An *Action* is a semantic object, not necessarily associated with a specific hardware control: for example, you would define a "Select" action, not an action for "press the A button". You would define an action "walk", not "left analogue stick position". The specific device input or output that an Action is associated with, is in the realm of *Bindings*, which are defined by *Interaction Profiles* (see below).

Actions are contextual. They are grouped within *Action Sets*, which are again semantic and app-specific. For example, you might have an Action Set for "gameplay" and a different Action Set for "pause menu". In more complex applications, you would create Action Sets for different situations - "driving car" and "walking" could be different Action Sets.

In this chapter, you'll learn how to create an Action Set containing multiple Actions of different types. You'll create a binding for your Actions with a simple controller profile, and optionally, with a profile specific to the device or devices you are testing.

************************************
4.2 Creating Actions and Action Sets
************************************

An OpenXR application has interactions with the user which can be user input to the application, or haptic output to the user. In this chapter, we will create some interactions and show how this system works. The interaction system uses three core concepts: Spaces, Actions, and Bindings.

At the end of your application class, add this code:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Actions
	:end-before: XR_DOCS_TAG_END_Actions
	:dedent: 4

Here, we have defined an Action Set: a group of related actions that are created together. The individual actions, such as `m_grabAction` and `m_palmPoseAction`, will belong to this set. For a pose action, we need an XrSpace, so `m_handPoseSpace[]` has been declared. And we'll keep a copy of the pose itself for each hand, which will change per-frame.

Action Sets are created before the session is initialized, so in Run(), after the call to GetSystemID(), add this line:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateActionSet
	:end-before: XR_DOCS_TAG_END_CallCreateActionSet
	:dedent: 8

After the definition of GetSystemID(), we'll add these helper functions that convert a string into an XrPath, and vice-versa. Add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateXrPath
	:end-before: XR_DOCS_TAG_END_CreateXrPath
	:dedent: 4

Now we will define the `CreateActionSet` function. Add the first part of this function after ``FromXrPath()``:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActionSet
	:end-before: XR_DOCS_TAG_END_CreateActionSet
	:dedent: 4

An Action Set is a group of actions that apply in a specific context. You might have an Action Set for when your XR game is showing a pause menu or control panel, and a different Action Set for in-game. There might be different Action Sets for different situations in an XR application: rowing in a boat, climbing a cliff, and so on.
So you can create multiple Action Sets, but we only need one for this example. The Action Set is created with a name, and a localized string for its description. Now add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActionLambda
	:end-before: XR_DOCS_TAG_END_CreateActionLambda
	:dedent: 4

Here we've created each action with a little local lambda function `CreateAction`. Each action has a name, a localized description, and the type of action it is. It also, optionally, has a list of sub-action paths. A sub-action is, essentially the same action on a different control device: left- or right-hand controllers for example.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActions
	:end-before: XR_DOCS_TAG_END_CreateActions
	:dedent: 4

Each Action and Action Set has both a name, for internal use, and a localized description to show to the end-user. This is because the user may want to re-map actions from the default controls, so there must be a human-readable name to show them.

*************************************
4.3 Interaction Profiles and Bindings
*************************************

4.3.1 Bindings and Profiles
---------------------------

As OpenXR is an API for many different devices, it needs to provide a way for you as a developer to refer to the various buttons, joysticks, inputs and outputs that a device may have, without needing to know in advance which device or devices the user will have.

To do this, OpenXR defines the concept of *interaction profiles*. An interaction profile is a collection of interactions supported by a given device and runtime, and defined by means of textual paths.

Each interaction is defined by a path with three components: the Profile Path, the User Path, and the Component Path. The Profile Path has the form ``"/interaction_profiles/<vendor_name>/<type_name>"``. The User Path is a string identifying the controller device, e.g. ``"/user/hand/left"``, or ``"/user/gamepad"``. The Component Path is a string identifying the specific input or output, e.g. ``"/input/select/click"`` or ``"/output/haptic_left_trigger"``.

For example, the Khronos Simple Controller Profile has the path:

``"/interaction_profiles/khr/simple_controller"``

It has user paths

``"/user/hand/left"``

and

``"/user/hand/right"``.

The component paths are:

* ``"/input/select/click"``
* ``"/input/menu/click"``
* ``"/input/grip/pose"``
* ``"/input/aim/pose"``
* ``"/output/haptic"``

Putting the three parts together, we might identify the select button on the left hand controller as:

``profile + user + component``

``"/interaction_profiles/khr/simple_controller" + "/user/hand/left" + "/input/select/click"``

``"/interaction_profiles/khr/simple_controller/user/hand/left/input/select/click"``

We will now show how to use these profiles in practice to suggest bindings between Actions and inputs or outputs.

4.3.2 Binding Interactions
--------------------------

We will set up bindings for the actions. A binding is a *suggested* correspondence between an action (which is app-defined), and the input/output on the user's devices. 

An XrPath is a 64-bit number that uniquely identifies any given forward-slash-delimited path string, allowing us to refer to paths without putting cumbersome string-handling in our runtime code. After the call to ``CreateActionSet()`` in ``Run()``, add the line:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallSuggestBindings
	:end-before: XR_DOCS_TAG_END_CallSuggestBindings
	:dedent: 8

After the definition of CreateActionSet(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SuggestBindings1
	:end-before: XR_DOCS_TAG_END_SuggestBindings1
	:dedent: 4

By means of a lambda function ``SuggestBindings``, we call into OpenXR with a given list of suggestions. Let's try this:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SuggestBindings2
	:end-before: XR_DOCS_TAG_END_SuggestBindings2
	:dedent: 4

Here, we create a proposed match-up between our app-specific actions, and XrPaths which refer to specific controls as interpreted by the Runtime. We call ``xrSuggestInteractionProfileBindings()``. If the user's device supports the given profile ( and ``"/interaction_profiles/khr/simple_controller"`` should usually be supported in any OpenXR runtime), it will recognize these paths and can map them to its own controls. If the user's device does not support a profile, the bindings will be ignored.
The suggested bindings are not guaranteed to be used: that's up to the runtime. Some runtimes allow users to override the default bindings, and OpenXR expects this.


For OpenXR hand controllers, we distinguish between a "grip pose", representing the orientation of the handle of the device, and an "aim pose" - which is oriented where the device "points". The relative orientations of these will vary between different controller configurations, so again, it's important to test with the devices you have. Let the runtime worry about adaptating to different devices that you haven't tested.

.. figure:: images/standard-poses.png
	:alt: OpenXR Standard Controller Poses
	:align: left

	Standard Grip and Aim poses for OpenXR Controllers.

The next part depends on what hardware you will be testing on. It's optional: the Khronos Simple Controller should work on any OpenXR runtime/device combination. But it has limitations - not least that there are no floating-point controls. If you have an Oculus Quest, whether building natively or running on PC VR via a streaming system, the native profile is called ``"/interaction_profiles/oculus/touch_controller"``, and you can insert the following:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SuggestTouchNativeBindings
	:end-before: XR_DOCS_TAG_END_SuggestTouchNativeBindings
	:dedent: 4

The main apparent difference is that the grab action is now analogue (``"squeeze/value"`` rather than ``"select/click"``). But you should never assume that the same path means exactly the same behaviour on different profiles. So again, only implement profiles that you can test with their associated hardware, and test every profile that you implement.

We now close out the function, and add ``RecordCurrentBindings()`` to report how the runtime has *actually* bound your actions to your devices.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_SuggestBindings3
	:end-before: XR_DOCS_TAG_END_SuggestBindings3
	:dedent: 4

In PollEvents(), in the switch case for ``XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED``, before the ``break;`` directive, insert this:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallRecordCurrentBindings
	:end-before: XR_DOCS_TAG_END_CallRecordCurrentBindings
	:dedent: 16

Now, on startup and if the interaction profile changes for any reason, the active binding will be reported.

Theory and Best Practices for Interaction Profiles
--------------------------------------------------

To get the best results from your application on the end-user's device, it is important to understand the key principles behind the Interaction Profile system. These are laid out in the OpenXR Guide (https://github.com/KhronosGroup/OpenXR-Guide/blob/main/chapters/goals_design_philosophy.md) but in brief:

* An application written for OpenXR should work without modification on device/runtime combination, even those created after the application has been written.
* An OpenXR device and runtime should work with any OpenXR application, even those not tested with that device.

The way this is achieved is as follows: usually, each device will have its own "native" profile, and should also support ``"khr/simple_controller"``. As a developer:

* You should *test the devices and runtimes you have*.
* You should *specify profile bindings for each device you have tested*.
* You should *not* implement profiles you have not tested.

It is the *runtime's responsibility* to support non-native profiles where possible, either automatically, or with the aid of user-specified rebinding. A device can support any number of interaction profiles, either the nine profiles defined in the OpenXR standard, or an extension profile (see https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#_adding_input_sources_via_extensions).

See also `semantic-path-interaction-profiles <https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#semantic-path-interaction-profiles>`_.


************************************
4.4 Using Actions in the application
************************************

Action Sets and Suggested Bindings are created before the session is initialized. There is session-specific setup to be done for our actions also. After the call to CreateSession(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallCreateActionPoses
	:end-before: XR_DOCS_TAG_END_CallCreateActionPoses
	:dedent: 8

Now after the definition of SuggestBindings(), add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CreateActionPoses
	:end-before: XR_DOCS_TAG_END_CreateActionPoses
	:dedent: 4

Here, we're creating the XrSpace that represents the hand pose actions. As with the reference space, we use an identity ``XrPosef`` to indicate that we'll take the pose as-is, without offsets.

Finally as far as action setup goes, we will attach the Action Set to the session. Add this function:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_AttachActionSet
	:end-before: XR_DOCS_TAG_END_AttachActionSet
	:dedent: 4

As you can see, it's possible here to attach multiple Action Sets. But ``xrAttachSessionActionSets`` can only be called *once* per session. You have to know what Action Sets you will be using before the session can start - xrBeginSession() is called from PollEvents() once all setup is complete and the application is ready to proceed.

Now, we must poll the actions, once per-frame. Add these two calls in RenderFrame() just before the call to RenderLayer():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallPollActions
	:end-before: XR_DOCS_TAG_END_CallPollActions
	:dedent: 12

And add this function after the definition of PollEvents():

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions
	:end-before: XR_DOCS_TAG_END_PollActions
	:dedent: 4

Here we enable the Action Set we're interested in (in our case we have only one), and tell OpenXR to prepare the actions' per-frame data with xrSyncActions().

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions2
	:end-before: XR_DOCS_TAG_END_PollActions2
	:dedent: 4

If, and only if the action is active, we use `xrLocateSpace` to obtain the current pose of the controller. We specify that we want this relative to our reference space `localOrStageSpace`, because this is the global space we're using for rendering. We'll use `leftGripPose` in the next section to render the
controller's position.

We'll add the grabbing Action.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions3
	:end-before: XR_DOCS_TAG_END_PollActions3
	:dedent: 4
	
Finally in this function, we'll add the haptic buzz behaviour, which has variable amplitude.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_PollActions4
	:end-before: XR_DOCS_TAG_END_PollActions4
	:dedent: 4

Now we've completed polling all the actions in the application. We will add two more functions to enable some interaction between the user and the 3D blocks:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_BlockInteraction
	:end-before: XR_DOCS_TAG_END_BlockInteraction
	:dedent: 4

*************************************
4.5 Rendering the Controller position
*************************************

We will now draw some geometry to represent the controller poses. We already have a mathematics library from a previous chapter, but we will need a couple more headers for ``std::min()``, ``std::max()`` and for generating pseudo-random colors.

Add this after ``#include <xr_linear_algebra.h>``:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_include_algorithm_random
	:end-before: XR_DOCS_TAG_END_include_algorithm_random
	:dedent: 0

After your declaration of ``m_pipeline`` and before ``XrActionSet m_actionSet``, we will declare some interactable 3D blocks. Add:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Objects
	:end-before: XR_DOCS_TAG_END_Objects
	:dedent: 4
	
Inside our ``CreateResources()`` method, locate where set the variable ``numberOfCuboids`` and update it as follows:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Update_numberOfCuboids
	:end-before: XR_DOCS_TAG_END_Update_numberOfCuboids
	:dedent: 8

We will render 64 interactable cubes, 2 cuboids representing the controllers and a further 2 for the floor and table from the previous chapter. After the call to create the graphics pipeline object, add the following coded that sets up the orientation, position and color of each interactable cube.

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_Setup_Blocks
	:end-before: XR_DOCS_TAG_END_Setup_Blocks
	:dedent: 8

Recall that we've already inserted a call to ``PollActions()`` in the function ``RenderFrame()``, so we're ready to render the controller position and input values. In ``RenderLayer()`` method, after we render the floor and table, render the controller positions and interactable cubes, add the following code:

.. literalinclude:: ../Chapter4/main.cpp
	:language: cpp
	:start-after: XR_DOCS_TAG_BEGIN_CallRenderCuboid2
	:end-before: XR_DOCS_TAG_END_CallRenderCuboid2
	:dedent: 12

Now build and run your application. You should see something like this:

.. figure:: images/Chapter4-Screenshot.png
	:alt: Chapter4 Screenshot of final result.
	:align: left
	:width: 99%

You should also be able to grab and move the cubes with the controllers.

**************************************
4.6 Checking for Connected Controllers
**************************************

Look again now at the function PollActions().

We specify which action to look at with the ``XrActionStateGetInfo`` struct. Then we use a type-specific call. For our boolean Grab Action, we call ``xrGetActionStateBoolean()`` to retrieve an ``XrActionStateBoolean`` struct. This specifies whether the value of the boolean is true or false, and we can use this to determine whether the user is pressing the specified button on the controller.
However, the struct ``XrActionStateBoolean`` also has a member called ``isActive``, which is true if the state of the action is actually being read. If it's false, the value of ``currentState`` is irrelevant - the polling failed. 

Similarly, ``XrActionStateFloat`` has a floating-point ``currentState`` value, which is valid if ``isActive`` is true. The struct has ``changedSinceLastSync``, which is true if the value changed between the previous and current calls to xrSync(). And it has ``lastChangeTime``, which is the time at which the value last changed. This allows us to be very precise about when the user pressed the button, and how long they held it down for. This could be used to detect "long presses", or double-clicks.

Careful use of this polling metadata will help you to create applications that are responsive and intuitive to use. Bear in mind as well that multiple physical controls could be bound to the same action, and the user could be using more than one controller at once. See the OpenXR spec for more details:

https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#multiple_inputs

***********
4.7 Summary
***********

In this chapter, you have learned about Actions, Controller Profiles, Bindings.

