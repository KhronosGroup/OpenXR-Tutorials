var graphicsApi = Cookies.get("graphicsApi")
console.log("graphicsApi = " + graphicsApi);
if (graphicsApi == "d3d11") {
	$(".d3d11").show();
	$(".d3d12").hide();
	$(".opengl").hide();
	$(".opengles").hide();
	$(".vulkan").hide();
}
if (graphicsApi == "d3d12") {
	$(".d3d11").hide();
	$(".d3d12").show();
	$(".opengl").hide();
	$(".opengles").hide();
	$(".vulkan").hide();
}
if (graphicsApi == "opengl") {
	$(".d3d11").hide();
	$(".d3d12").hide();
	$(".opengl").show();
	$(".opengles").hide();
	$(".vulkan").hide();
}
if (graphicsApi == "opengles") {
	$(".d3d11").hide();
	$(".d3d12").hide();
	$(".opengl").hide();
	$(".opengles").show();
	$(".vulkan").hide();
}
if (graphicsApi == "vulkan") {
	$(".d3d11").hide();
	$(".d3d12").hide();
	$(".opengl").hide();
	$(".opengles").hide();
	$(".vulkan").show();
}