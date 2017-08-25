function Component() {
}
Component.prototype.createOperations = function() {
    component.createOperations();
	if (systemInfo.productType === "windows") {
		component.addOperation("Execute", "{0,1638}", "@TargetDir@\\vc_redist.x86.exe", "/install", "/passive", "/norestart");
		component.addOperation("Delete", "@TargetDir@\\vc_redist.x86.exe");
		component.addOperation("CreateShortcut", "@TargetDir@/_SquidStat.exe", "@DesktopDir@/SquidStat.lnk");
	}
}