const fs = require("fs");
function filterDir(dir) {

	if (dir.name.substring(0, 1) == ".")
		return false
	return true
}
function filterFile(file) {
	if (file.name.split(".").pop() != "c")
		return false
	return true
}

function getFiles(workingDir, openFile) {
	let sourceDir = `${workingDir}\\src`
	let mainFile = `${workingDir}\\main\\main.c`;

	return getCFiles(sourceDir).concat([mainFile])
}
function getCFiles(pathStart) {
	//console.log("looking in dir " + pathStart)
	var filesToCheck = fs.readdirSync(pathStart, { withFileTypes: true });
	//console.log(filesToCheck)
	var files = []
	for (const file of filesToCheck) {
		let fullPath = file.parentPath + "\\" + file.name;

		if (true) {
			if (file.isDirectory()) {
				if (filterDir(file))
					files = files.concat(getCFiles(fullPath));
			}
			else if (filterFile(file))
				files.push(fullPath);
		}
	}
	return files;
}

console.log(getFiles("D:\\coding\\TerminalFactory", "D:\\coding\\TerminalFactory\\main\\main.c").reduce((string, item) => `${string} ${item}`))