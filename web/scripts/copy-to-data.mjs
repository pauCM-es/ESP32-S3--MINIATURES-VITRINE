import { mkdir, readdir, rm, copyFile } from "node:fs/promises";
import path from "node:path";
import process from "node:process";

async function copyDir(srcDir, dstDir) {
	await mkdir(dstDir, { recursive: true });
	const entries = await readdir(srcDir, { withFileTypes: true });
	for (const entry of entries) {
		const srcPath = path.join(srcDir, entry.name);
		const dstPath = path.join(dstDir, entry.name);
		if (entry.isDirectory()) {
			await copyDir(srcPath, dstPath);
		} else if (entry.isFile()) {
			await mkdir(path.dirname(dstPath), { recursive: true });
			await copyFile(srcPath, dstPath);
		}
	}
}

async function main() {
	// web/dist -> ../data
	const webRoot = process.cwd();
	const distDir = path.join(webRoot, "dist");
	const dataDir = path.resolve(webRoot, "..", "data");

	// Only replace the SPA build outputs, never touch data/update/.
	// Vite defaults to dist/index.html + dist/assets/*.
	const srcIndex = path.join(distDir, "index.html");
	const srcAssets = path.join(distDir, "assets");

	const dstIndex = path.join(dataDir, "index.html");
	const dstAssets = path.join(dataDir, "assets");

	await mkdir(dataDir, { recursive: true });

	// Clean old assets
	await rm(dstAssets, { recursive: true, force: true });

	// Copy index.html
	await copyFile(srcIndex, dstIndex);

	// Copy assets directory if present
	try {
		await copyDir(srcAssets, dstAssets);
	} catch {
		// If there are no assets, that's fine.
	}

	console.log(`Copied SPA build to ${dataDir}`);
	console.log(`- ${path.relative(process.cwd(), dstIndex)}`);
	console.log(`- ${path.relative(process.cwd(), dstAssets)}`);
	console.log("Note: data/update/ is preserved.");
}

main().catch((err) => {
	console.error(err);
	process.exit(1);
});
