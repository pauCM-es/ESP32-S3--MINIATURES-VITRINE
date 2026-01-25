import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

const proxyTarget = process.env.VITE_PROXY_TARGET;

// https://vite.dev/config/
export default defineConfig({
	plugins: [react()],
	server: proxyTarget
		? {
				proxy: {
					"/api": {
						target: proxyTarget,
						changeOrigin: true,
					},
					"/ws": {
						target: proxyTarget,
						ws: true,
						changeOrigin: true,
					},
					"/update": {
						target: proxyTarget,
						changeOrigin: true,
					},
				},
			}
		: undefined,
});
