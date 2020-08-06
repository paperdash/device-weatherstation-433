const webpack = require('webpack');
const CompressionPlugin = require('compression-webpack-plugin');

module.exports = {
	"outputDir": "../data/dist",
	"filenameHashing": false,
	"productionSourceMap": false,
	"transpileDependencies": [
		"vuetify"
	],
	devServer: {
		proxy: {
			'^/': {
				target: 'http://192.168.178.88:80',
				ws: true,
				changeOrigin: true
			},
		}
	},
	configureWebpack: config => {
		if (process.env.NODE_ENV === 'production') {
			// optimize build for esp
			return {
				plugins: [
					// reduce total size of the app
					new webpack.optimize.LimitChunkCountPlugin({
						maxChunks: 1
					}),
					// use only pre compressed files
					new CompressionPlugin({
						deleteOriginalAssets: true
					})
				]
			}
		} else {
			// mutate for development...
		}
	}
}