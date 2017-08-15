function Component() {
}
Component.prototype.createOperations = function() {
    component.createOperations();
	component.addElevatedOperation("Execute", "cmd.exe", "/C", "@TargetDir@/vc_redist.x86.install.cmd");
}