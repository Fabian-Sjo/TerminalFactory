const { execSync } = require("child_process");
const path = require("path");
const fs = require("fs");


const workspaceFolder = process.argv[2];
const openFile = process.argv[3];
const outDir = path.join(workspaceFolder, "out");


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
function isInDirectory(filePath, directoryPath) {
	const file = path.resolve(filePath);
	const dir = path.resolve(directoryPath);

	const relative = path.relative(dir, file);

	return relative &&
		!relative.startsWith('..') &&
		!path.isAbsolute(relative);
}
function getFiles(workingDir, openFile) {
	let sourceDir = `${workingDir}\\src`
	let mainFile = `${workingDir}\\main\\main.c`;
	if (!isInDirectory(openFile, sourceDir))
		mainFile = openFile;
	
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
let files = getFiles(workspaceFolder, openFile)
let fileString = files.reduce((string, item) => `${string} ${item}`)
let command;

console.log("Building FULL project");
console.log("Main file:"+files[files.length - 1]);
console.log("\n\n\n\n");
command = `cl.exe /Zi /Od /W3 /EHsc /std:c11 /Foout\\ /Feout\\program.exe ` +
	fileString


console.log(command);
setTimeout((() => execSync(command, { stdio: "inherit" })), 0);