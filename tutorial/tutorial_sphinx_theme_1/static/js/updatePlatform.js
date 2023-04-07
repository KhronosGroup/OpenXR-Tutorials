var platform = Cookies.get("platform")
console.log("platform = " + platform);
if (platform == "windows") {
	$(".linux").hide();
	$(".android").hide();
	$(".windows").show();
	$(".windows-linux").show();
}
if (platform == "linux") {
	$(".linux").show();
	$(".android").hide();
	$(".windows").hide();
	$(".windows-linux").show();
}
if (platform == "android") {
	$(".linux").hide();
	$(".android").show();
	$(".windows").hide();
	$(".windows-linux").hide();
}