OpenXR Tutorial
===============

This tutorial will teach you how to use the OpenXR API. OpenXR is an API managed by the Khronos Group which provides a cross-platform way for applications to interact with immersive devices. This includes virtual reality (VR) headsets, augmented reality (AR) devices, motion controllers and more.

An API is an application programming interface, a set of rules for how a software system can be interfaced-with by other systems. There is no specific library or code that is OpenXR, but rather there are multiple implementations (or runtimes), which might be provided by hardware manufacturers or by software companies; there are even open source runtimes for OpenXR. Whichever runtime you use, if it's compliant with the OpenXR specification, your OpenXR-compliant code will work with it.

This tutorial introduces programming with OpenXR. If you use a game engine such as Unreal Engine, Unity or Godot, you may not need to interact directly with OpenXR: the engine abstracts this away. However, it may be beneficial to learn the underlying concepts.

OpenXR is designed to work with various different graphics APIs: Vulkan, Direct3D 11, Direct3D 12, OpenGL and OpenGL ES are all supported at present. You will need some familiarity with your graphics API of choice to get the most out of this tutorial.

Other prerequisites are:

* A Windows or Linux PC to compile the code.
* An IDE (integrated development environment) that supports your target device: either Visual Studio 2017 or later; or Android Studio.
* Some XR hardware to run on: either an XR device that connects to your PC, or an Android-based device like a Meta Quest or Pico Neo. It is also possible to use `Monado <https://monado.freedesktop.org/>`_ to emulate XR hardware.
* CMake 3.22.1 is required.
* Python 3.6+ is required to build the OpenXR-SDK-Source repository. See `OpenXR-SDK-Source/BUILDING.md <https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/BUILDING.md>`_.

Tutorial Structure
------------------

The tutorial is divided into several main chapters. Each chapter walks you through a part of the process of creating an XR application and assumes that the previous chapters have been completed.

Explanations of key concepts are given throughout the chapters to provide deeper insight into the workings of OpenXR. The code snippets can be copied directly into your project as you follow the tutorial, and explanations for the code are generally given after the code snippets.

At the end of each chapter there is a link to download the full example source code up to that point.

The full source code for this tutorial can also be downloaded from `https://github.com/KhronosGroup/OpenXR-Tutorials <https://github.com/KhronosGroup/OpenXR-Tutorials>`_.

* In the Introduction, we describe the basic concepts underpinning OpenXR, and show you how to set up a PC or Android development environment to build the code.
* In Setup, we show how to initialize OpenXR and connect your program to your XR device.
* In Graphics, we setup the swapchains and render graphics to the views.
* In Actions, we use OpenXR's action system to bind input to application specific actions allowing the user to interact with the XR device.
* In Extensions, we demonstrate various extensions that you may find helpful.

To begin the tutorial, select the platform and graphics API you want to use:

.. raw:: html
	
	<div class="wide-version-table docutils container">
		<table class="docutils align-default">
			<colgroup>
				<col style="width: 4%" />
				<col style="width: 22%" />
				<col style="width: 22%" />
				<col style="width: 26%" />
				<col style="width: 25%" />
			</colgroup>
			<thead>
				<tr class="row-odd"><th class="head"></th>
				<th class="head"><p>D3D11</p></th>
				<th class="head"><p>D3D12</p></th>
				<th class="head"><p>OpenGL or OpenGL&nbsp;ES</p></th>
				<th class="head"><p>Vulkan</p></th>
				</tr>
			</thead>
			<tbody>
				<tr class="row-even"><td><p>Android</p></td>
					<td></td>
					<td></td>
					<td><p><a class="reference external" href="/android/opengles/index.html">Android, OpenGL&nbsp;ES</a></p></td>
					<td><p><a class="reference external" href="/android/vulkan/index.html">Android, Vulkan</a></p></td>
				</tr>
					<tr class="row-odd"><td><p>Linux</p></td>
					<td></td>
					<td></td>
					<td><p><a class="reference external" href="/linux/opengl/index.html">Linux, OpenGL</a></p></td>
				<td><p><a class="reference external" href="/linux/vulkan/index.html">Linux, Vulkan</a></p></td>
				</tr>
				<tr class="row-even"><td><p>Windows</p></td>
					<td><p><a class="reference external" href="/windows/d3d11/index.html">Windows, D3D11</a></p></td>
					<td><p><a class="reference external" href="/windows/d3d12/index.html">Windows, D3D12</a></p></td>
					<td><p><a class="reference external" href="/windows/opengl/index.html">Windows, OpenGL</a></p></td>
					<td><p><a class="reference external" href="/windows/vulkan/index.html">Windows, Vulkan</a></p></td>
				</tr>
				</tbody>
		</table>
	</div>
	<div class="narrow-version-table docutils container">
		<ul class="simple">
			<li><dl class="simple">
				<dt>Android</dt><dd><ul>
				<li><p><a class="reference external" href="/android/opengles/index.html">Android, OpenGL&nbsp;ES</a></p></li>
				<li><p><a class="reference external" href="/android/vulkan/index.html">Android, Vulkan</a></p></li>
				</ul>
				</dd>
				</dl>
			</li>
			<li><dl class="simple">
				<dt>Linux</dt><dd><ul>
				<li><p><a class="reference external" href="/linux/opengl/index.html">Linux, OpenGL</a></p></li>
				<li><p><a class="reference external" href="/linux/vulkan/index.html">Linux, Vulkan</a></p></li>
				</ul>
				</dd>
				</dl>
			</li>
			<li><dl class="simple">
				<dt>Windows</dt><dd><ul>
				<li><p><a class="reference external" href="/windows/d3d11/index.html">Windows, D3D11</a></p></li>
				<li><p><a class="reference external" href="/windows/d3d12/index.html">Windows, D3D12</a></p></li>
				<li><p><a class="reference external" href="/windows/opengl/index.html">Windows, OpenGL</a></p></li>
				<li><p><a class="reference external" href="/windows/vulkan/index.html">Windows, Vulkan</a></p></li>
				</ul>
				</dd>
				</dl>
			</li>
		</ul>
	</div>

At any time you can change platform/API by clicking the link at the top-right of the page.	

.. only:: OPENXR_SUBSITE

	.. toctree::
		:maxdepth: 5
		:caption: Contents:

		1-introduction
		2-setup
		3-graphics
		4-actions
		5-extensions
		6-next-steps