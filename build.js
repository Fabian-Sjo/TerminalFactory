const { execSync } = require("child_process");
const path = require("path");
const fs = require("fs");
const { spawn } = require("child_process");
const { exit } = require("process");


const workspaceFolder = process.argv[2];
const openFile = process.argv[3];
const outDir = path.join(workspaceFolder, "out");

/**
 * @param {{
 *   text: string;
 *   color?: "black" | "red" | "green" | "yellow" | "blue" | "magenta" | "cyan" | "white" |
 *            "brightBlack" | "brightRed" | "brightGreen" | "brightYellow" |
 *            "brightBlue" | "brightMagenta" | "brightCyan" | "brightWhite";
 *   bold?: boolean;
 *   underline?: boolean;
 * }} options
 * @returns {string}
 */
function styleText({ text, color, bold = false, underline = false }) {
	const colors = {
		black: 30,
		red: 31,
		green: 32,
		yellow: 33,
		blue: 34,
		magenta: 35,
		cyan: 36,
		white: 37,
		brightBlack: 90,
		brightRed: 91,
		brightGreen: 92,
		brightYellow: 93,
		brightBlue: 94,
		brightMagenta: 95,
		brightCyan: 96,
		brightWhite: 97,
	};

	const codes = [];

	if (bold) codes.push(1);
	if (underline) codes.push(4);
	if (color) codes.push(colors[color]);

	if (codes.length === 0) return text;

	return `\x1b[${codes.join(";")}m${text}\x1b[0m`;
}
function parseChunk(chunk) {
	const regex = /(.*\.[hc])\((\d+)\)\s*:\s*(warning|error)\s*(\w*):\s*(.*)/;
	const parsed = chunk.match(regex);
	if (parsed == null)
		return chunk
	if (parsed.length < 5)
		return chunk
	const isError = parsed[3] == "error";
	const wholeLine = parsed[0];
	const path = styleText({
		text: parsed[1],
		color: "white"
	});
	const line = styleText({
		text: parsed[2],
		color: "white"
	});

	const error = isError ?
		styleText({
			text: parsed[3],
			color: "red"
		}) : styleText({
			text: parsed[3],
			color: "yellow"
		});
	const errorCode = styleText({
		text: parsed[4],
		color: "black"
	});
	const errorText = isError ?
		styleText({
			text: parsed[5],
			color: "red"
		}) : styleText({
			text: parsed[5],
			color: "yellow"
		});
	return `${path}(${line}) ${error} : ${errorText}\n`;
}


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

console.log("Building project");
console.log("Main file: " + styleText({
	text: files[files.length - 1],
	color: "green",
	bold: true,
}));
console.log("\n\n");
command = `cl.exe /Zi /Od /W3 /EHsc /std:c11 /Foout\\ /Feout\\program.exe ` +
	fileString


console.log(styleText({
	text: command,
	color: "black",
	bold: true,
}));

const child = spawn(command, [], {
	shell: true,
});

let output = "";

child.stdout.setEncoding("utf8");
child.stdout.on("data", chunk => {
	output += chunk;
	process.stdout.write(parseChunk(chunk)); // prints immediately
});

child.stderr.setEncoding("utf8");
child.stderr.on("data", chunk => {
	//process.stderr.write(chunk);
});

child.on("close", code => {
	console.log("Exit code:", code);
	exit(code)
	//console.log("Full output:", output);
});
