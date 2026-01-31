import "./App.css";
import { Link, Route, Routes } from "react-router-dom";
import StatusPage from "./pages/StatusPage";
import UpdatesPage from "./pages/UpdatesPage";
import LedsPage from "./pages/LedsPage";

function App() {
	return (
		<div className="app">
			<header className="topbar">
				<div className="brand">ESP32-S3 Smart Vitrine</div>
				<nav className="nav">
					<Link to="/">Status</Link>
					<Link to="/leds">LEDs</Link>
					<Link to="/updates">Updates</Link>
				</nav>
			</header>

			<main className="content">
				<Routes>
					<Route
						path="/"
						element={<StatusPage />}
					/>
					<Route
						path="/leds"
						element={<LedsPage />}
					/>
					<Route
						path="/updates"
						element={<UpdatesPage />}
					/>
				</Routes>
			</main>
		</div>
	);
}

export default App;
