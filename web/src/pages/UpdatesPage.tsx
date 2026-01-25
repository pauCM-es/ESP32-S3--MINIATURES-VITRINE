export default function UpdatesPage() {
	return (
		<div className="card">
			<h2 style={{ marginTop: 0 }}>Updates</h2>

			<p className="muted">
				This device supports firmware OTA via{" "}
				<code>POST /update/firmware</code>.
			</p>

			<div
				className="row"
				style={{ marginBottom: 10 }}>
				<a
					href="/update"
					style={{ padding: "8px 10px" }}>
					Open Firmware Upload Page
				</a>
				<a
					href="/api/info"
					style={{ padding: "8px 10px" }}>
					View /api/info
				</a>
			</div>

			<h3>Build → Upload flow</h3>
			<ol className="muted">
				<li>
					Build firmware in PlatformIO (creates{" "}
					<code>firmware.bin</code> under{" "}
					<code>.pio/build/&lt;env&gt;/</code>).
				</li>
				<li>
					Open <code>/update</code>, select the generated{" "}
					<code>.bin</code>, press Upload.
				</li>
				<li>The ESP32 will reboot after a successful update.</li>
			</ol>

			<h3>CLI example (Windows)</h3>
			<pre className="muted">
				{`curl.exe -X POST http://<device-ip>/update/firmware \
  -H "Content-Type: application/octet-stream" \
  --data-binary "@firmware.bin"`}
			</pre>
		</div>
	);
}
