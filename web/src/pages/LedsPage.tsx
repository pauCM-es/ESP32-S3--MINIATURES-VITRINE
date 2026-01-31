import { useEffect, useMemo, useRef, useState } from "react";

type WsState = "disconnected" | "connecting" | "connected";

function wsUrl(): string {
	const proto = window.location.protocol === "https:" ? "wss" : "ws";
	return `${proto}://${window.location.host}/ws`;
}

function hexToRgb(hex: string): { r: number; g: number; b: number } {
	const m = /^#?([0-9a-f]{6})$/i.exec(hex);
	if (!m) return { r: 255, g: 255, b: 255 };
	const n = parseInt(m[1], 16);
	return { r: (n >> 16) & 0xff, g: (n >> 8) & 0xff, b: n & 0xff };
}

export default function LedsPage() {
	const [wsState, setWsState] = useState<WsState>("disconnected");
	const [wsError, setWsError] = useState<string | null>(null);
	const [lastRx, setLastRx] = useState<string | null>(null);

	const wsRef = useRef<WebSocket | null>(null);

	const [brightness, setBrightness] = useState(40);
	const [pixelIndex, setPixelIndex] = useState(0);
	const [pixelColor, setPixelColor] = useState("#ffffff");
	const rgb = useMemo(() => hexToRgb(pixelColor), [pixelColor]);

	const [modeName, setModeName] = useState<"standby" | "focus" | "selected">(
		"focus",
	);
	const [modeIndex, setModeIndex] = useState(0);
	const [standbyBrightness, setStandbyBrightness] = useState(30);

	function connect() {
		setWsError(null);
		setWsState("connecting");

		try {
			const ws = new WebSocket(wsUrl());
			wsRef.current = ws;

			ws.onopen = () => {
				setWsState("connected");
			};
			ws.onclose = () => {
				setWsState("disconnected");
			};
			ws.onerror = () => {
				setWsError("WebSocket error");
			};
			ws.onmessage = (ev) => {
				setLastRx(String(ev.data));
			};
		} catch (e) {
			setWsError(e instanceof Error ? e.message : String(e));
			setWsState("disconnected");
		}
	}

	function disconnect() {
		wsRef.current?.close();
		wsRef.current = null;
	}

	function send(obj: unknown) {
		const ws = wsRef.current;
		if (!ws || ws.readyState !== WebSocket.OPEN) {
			setWsError("Not connected");
			return;
		}
		ws.send(JSON.stringify(obj));
	}

	useEffect(() => {
		connect();
		return () => {
			disconnect();
		};
		// eslint-disable-next-line react-hooks/exhaustive-deps
	}, []);

	return (
		<div className="card">
			<h2 style={{ marginTop: 0 }}>LED Control</h2>

			<div
				className="row"
				style={{ marginBottom: 10 }}>
				<strong>Status:</strong>
				<span>
					{wsState === "connected"
						? "Connected"
						: wsState === "connecting"
							? "Connecting…"
							: "Disconnected"}
				</span>
				<span className="muted">(via /ws)</span>
				<button
					onClick={connect}
					disabled={wsState === "connected"}>
					Connect
				</button>
				<button
					onClick={disconnect}
					disabled={wsState !== "connected"}>
					Disconnect
				</button>
			</div>

			{wsError ? (
				<p style={{ color: "#d1242f" }}>Error: {wsError}</p>
			) : null}

			<div
				className="card"
				style={{ marginTop: 12 }}>
				<h3 style={{ marginTop: 0 }}>Brightness</h3>
				<div className="row">
					<input
						type="range"
						min={0}
						max={100}
						value={brightness}
						onChange={(e) => setBrightness(Number(e.target.value))}
						style={{ width: 240 }}
					/>
					<strong>{brightness}%</strong>
					<button
						onClick={() =>
							send({
								type: "led",
								cmd: "brightness",
								value: brightness,
							})
						}>
						Apply
					</button>
					<button onClick={() => send({ type: "led", cmd: "clear" })}>
						Clear
					</button>
				</div>
				<p
					className="muted"
					style={{ marginBottom: 0 }}>
					Sends{" "}
					<code>{`{"type":"led","cmd":"brightness","value":N}`}</code>
				</p>
			</div>

			<div
				className="card"
				style={{ marginTop: 12 }}>
				<h3 style={{ marginTop: 0 }}>Set Pixel</h3>
				<div className="row">
					<label>
						Index:&nbsp;
						<input
							type="number"
							min={0}
							value={pixelIndex}
							onChange={(e) =>
								setPixelIndex(Number(e.target.value))
							}
							style={{ width: 80 }}
						/>
					</label>
					<label>
						Color:&nbsp;
						<input
							type="color"
							value={pixelColor}
							onChange={(e) => setPixelColor(e.target.value)}
						/>
					</label>
					<span className="muted">
						RGB: {rgb.r}, {rgb.g}, {rgb.b}
					</span>
					<button
						onClick={() =>
							send({
								type: "led",
								cmd: "pixel",
								index: pixelIndex,
								r: rgb.r,
								g: rgb.g,
								b: rgb.b,
							})
						}>
						Send
					</button>
				</div>
				<p
					className="muted"
					style={{ marginBottom: 0 }}>
					Sends{" "}
					<code>{`{"type":"led","cmd":"pixel","index":I,"r":R,"g":G,"b":B}`}</code>
				</p>
			</div>

			<div
				className="card"
				style={{ marginTop: 12 }}>
				<h3 style={{ marginTop: 0 }}>Mode</h3>
				<div className="row">
					<label>
						Mode:&nbsp;
						<select
							value={modeName}
							onChange={(e) =>
								setModeName(
									e.target.value as
										| "standby"
										| "focus"
										| "selected",
								)
							}>
							<option value="standby">standby</option>
							<option value="focus">focus</option>
							<option value="selected">selected</option>
						</select>
					</label>

					{modeName === "standby" ? (
						<label>
							Brightness:&nbsp;
							<input
								type="number"
								min={0}
								max={100}
								value={standbyBrightness}
								onChange={(e) =>
									setStandbyBrightness(Number(e.target.value))
								}
								style={{ width: 80 }}
							/>
						</label>
					) : (
						<label>
							Index:&nbsp;
							<input
								type="number"
								min={0}
								value={modeIndex}
								onChange={(e) =>
									setModeIndex(Number(e.target.value))
								}
								style={{ width: 80 }}
							/>
						</label>
					)}

					<button
						onClick={() => {
							if (modeName === "standby") {
								send({
									type: "led",
									cmd: "mode",
									name: "standby",
									brightness: standbyBrightness,
								});
							} else {
								send({
									type: "led",
									cmd: "mode",
									name: modeName,
									index: modeIndex,
								});
							}
						}}>
						Apply
					</button>
				</div>
				<p
					className="muted"
					style={{ marginBottom: 0 }}>
					Sends <code>{`{"type":"led","cmd":"mode",...}`}</code>
				</p>
			</div>

			<div
				className="card"
				style={{ marginTop: 12 }}>
				<h3 style={{ marginTop: 0 }}>Last Response</h3>
				<pre className="muted">{lastRx ?? "(none)"}</pre>
			</div>

			<p
				className="muted"
				style={{ marginBottom: 0, marginTop: 12 }}>
				Dev tip: set <code>VITE_PROXY_TARGET</code> to your device URL
				and run
				<code>npm run dev</code>.
			</p>
		</div>
	);
}
