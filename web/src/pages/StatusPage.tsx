import { useEffect, useState } from "react";

type ApiInfo = Record<string, unknown>;

export default function StatusPage() {
	const [loading, setLoading] = useState(false);
	const [error, setError] = useState<string | null>(null);
	const [info, setInfo] = useState<ApiInfo | null>(null);

	async function refresh() {
		setLoading(true);
		setError(null);
		try {
			const res = await fetch("/api/info", { cache: "no-store" });
			if (!res.ok) {
				throw new Error(`HTTP ${res.status}`);
			}
			const json = (await res.json()) as ApiInfo;
			setInfo(json);
		} catch (e) {
			setInfo(null);
			setError(e instanceof Error ? e.message : String(e));
		} finally {
			setLoading(false);
		}
	}

	useEffect(() => {
		refresh();
	}, []);

	return (
		<div className="card">
			<h2 style={{ marginTop: 0 }}>Device Status</h2>

			<div
				className="row"
				style={{ marginBottom: 10 }}>
				<button
					onClick={refresh}
					disabled={loading}>
					{loading ? "Refreshing…" : "Refresh"}
				</button>
				<span className="muted">
					Uses <code>/api/info</code>
				</span>
			</div>

			{error ? (
				<p style={{ color: "#d1242f" }}>
					Error: {error}
					<br />
					<span className="muted">
						Tip: in dev mode, set <code>VITE_PROXY_TARGET</code> to
						your ESP IP and restart <code>npm run dev</code>.
					</span>
				</p>
			) : null}

			<pre className="muted">
				{info
					? JSON.stringify(info, null, 2)
					: loading
						? "Loading…"
						: "No data"}
			</pre>
		</div>
	);
}
