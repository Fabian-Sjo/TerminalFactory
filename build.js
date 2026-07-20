const { execSync } = require("child_process");
const path = require("path");
const fs = require("fs");
const { spawn } = require("child_process");
const { exit } = require("process");


const workspaceFolder = process.argv[2];
const fallbackMain = process.argv[3];
const openFile = process.argv[4];
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
const ChunkType = {
	ERROR: "ERROR",
	WARNING: "WARNING",
	OTHER: "OTHER",
}
function parseLinker(chunk) {
	const regex = /(.+\.(?:obj|exe))\s*:\s*(\S+)\s*(.*)/;
	const parsed = chunk.match(regex);
	if (parsed == null)
		return {
			type: ChunkType.OTHER,
			str: chunk
		}
	if (parsed.length < 2)
		return {
			type: ChunkType.OTHER,
			str: chunk
		}
	let type = ChunkType.OTHER
	switch (parsed[2]) {
		case "fatal":
		case "error":
			type = ChunkType.ERROR
			break;
		default:
			type = ChunkType.OTHER
			break;
	}
	const wholeLine = parsed[0];
	const path = styleText({
		text: parsed[1],
		color: "white"
	});

	const error = type == ChunkType.ERROR ?
		styleText({
			text: parsed[2],
			color: "red"
		}) : styleText({
			text: parsed[2],
			color: "yellow"
		});

	const errorText = type == ChunkType.ERROR ?
		styleText({
			text: parsed[3],
			color: "red"
		}) : styleText({
			text: parsed[3],
			color: "yellow"
		});
	return {
		type: type,
		str: `${path} ${error} : ${errorText}`
	};
}
function parseCompiler(chunk) {

	const regex = /(.*\.[hc])\((\d+)\)\s*:\s*(warning|error)\s*(\w*):\s*(.*)/;
	const parsed = chunk.match(regex);
	if (parsed == null)
		return {
			type: ChunkType.OTHER,
			str: chunk
		}
	if (parsed.length < 5)
		return {
			type: ChunkType.OTHER,
			str: chunk
		}
	let type = ChunkType.OTHER
	switch (parsed[3]) {
		case "error":
			type = ChunkType.ERROR
			break;
		case "warning":
			type = ChunkType.WARNING
			break;
		default:
			type = ChunkType.OTHER
			break;
	}
	const wholeLine = parsed[0];
	const path = styleText({
		text: parsed[1],
		color: "white"
	});
	const line = styleText({
		text: parsed[2],
		color: "white"
	});

	const error = type == ChunkType.ERROR ?
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
	const errorText = type == ChunkType.ERROR ?
		styleText({
			text: parsed[5],
			color: "red"
		}) : styleText({
			text: parsed[5],
			color: "yellow"
		});
	return {
		type: type,
		str: `${path}(${line}) ${error} : ${errorText}`
	};
}

function isInDirectory(filePath, directoryPath) {
	const file = path.resolve(filePath);
	const dir = path.resolve(directoryPath);

	const relative = path.relative(dir, file);

	return relative &&
		!relative.startsWith('..') &&
		!path.isAbsolute(relative);
}
const foundFiles = new Set();

function getImports(filePath) {
	if (filePath == null)
		return [];

	filePath = path.normalize(filePath);

	if (!fs.existsSync(filePath))
		return [];

	if (foundFiles.has(filePath))
		return [];

	foundFiles.add(filePath);
	getImports(getCFile(filePath));
	const data = fs.readFileSync(filePath, "utf8");

	for (const line of data.split(/\r?\n/)) {
		const regex = /^#include "(.*h)"$/;
		const parsed = line.match(regex);

		if (parsed != null) {
			const fullPath = path.resolve(path.dirname(filePath), parsed[1]);

			getImports(fullPath);
		}
	}
}
function getCFile(headerPath) {
	const cPath = headerPath.replace(/\.[^.]+$/, ".c");

	if (fs.existsSync(cPath)) {
		return cPath;
	}

	return null;
}
function isMainFile(filepath) {
	if (filepath == null)
		return false;
	const data = fs.readFileSync(filepath, "utf8");

	// Remove comments
	const clean = data
		.replace(/\/\/.*$/gm, "")
		.replace(/\/\*[\s\S]*?\*\//g, "");

	return /\bint\s+main\s*\([^)]*\)/.test(clean);
}
let mainFile = null
if (isMainFile(openFile))
	mainFile = openFile
else if (isMainFile(fallbackMain))
	mainFile = fallbackMain
if (mainFile == null)
	return -1
getImports(mainFile)
let files = Array.from(foundFiles).filter(p => path.resolve(p) !== path.resolve(mainFile)).filter(p => p.endsWith(".c"))
let fileString = files.reduce((string, item) => `${string} ${item}`)
let command;

console.log("Building project");
console.log("Main file: " + styleText({
	text: mainFile,
	color: "green",
	bold: true,
}));

console.log("\nImports:");
files.forEach(file => console.log(file))
command = `cl.exe /Zi /Od /W3 /EHsc /std:c11 /Foout\\ /Feout\\program.exe ${mainFile} ${fileString}`


console.log(styleText({ text: command, color: "black", bold: true, }));
const child = spawn(command, [], {
	shell: true,
});
let spinnerCount = 0
let spinner = "◜◝◞◟"
const interval = setInterval(() => {
	process.stdout.write(`${spinner.charAt(spinnerCount++ % spinner.length)}\r`)
}, 200);





let output = {
	errors: [],
	warnings: [],
	other: []
};

child.stdout.setEncoding("utf8");
let stage = "compiler";
child.stdout.on("data", chunk => {
	const linkerRegex = /^Microsoft \(R\) Incremental Linker Version/;
	if (chunk.match(linkerRegex)) {
		stage = "linker"
	}
	let out;
	switch (stage) {
		case "compiler":
			out = parseCompiler(chunk)
			break;
		case "linker":
			out = parseLinker(chunk)
			break;

		default:
			out = null
			break;
	}
	if (out.type == ChunkType.ERROR)
		output.errors.push(out)
	if (out.type == ChunkType.WARNING)
		output.warnings.push(out)
	if (out.type == ChunkType.OTHER)
		output.other.push(out)
});

child.stderr.setEncoding("utf8");
child.stderr.on("data", chunk => {
	//process.stderr.write(chunk);
});

child.on("close", code => {
	clearInterval(interval)
	console.log(styleText({
		text: "WARNINGS----------------------------",
		color: "yellow"
	}))
	output.warnings.forEach(element => {
		console.log(`\t${element.str}`)
	});
	console.log(styleText({
		text: "ERRORS----------------------------",
		color: "red"
	}))
	output.errors.forEach(element => {
		console.log(`\t${element.str}`)
	});
	console.log("Exit code:", code);
	//fs.readFileSync(0, "utf8");
	exit(code)
	//console.log("Full output:", output);
});
