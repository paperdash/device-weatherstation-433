const webpack = require('webpack')
const CompressionPlugin = require('compression-webpack-plugin')

module.exports = {
  outputDir: '../data/dist',
  filenameHashing: false,
  productionSourceMap: false,
  transpileDependencies: [
    'vuetify',
  ],
  devServer: {
    proxy: {
      '^/': {
        target: 'http://paperdash-weatherstation.local:80',
        ws: true,
        changeOrigin: true,
      },
    },
  },
  chainWebpack: (config) => {
    const svgRule = config.module.rule('svg')

    svgRule.uses.clear()

    svgRule
      .use('babel-loader')
      .loader('babel-loader')
      .end()
      .use('vue-svg-loader')
      .loader('vue-svg-loader')
      .options({
        svgo: {
          plugins: [
            { cleanupIDs: false },
            { collapseGroups: false },
            { removeEmptyContainers: false },
          ],
        },
      })
  },
  configureWebpack: () => {
    if (process.env.NODE_ENV === 'production') {
      // optimize build for esp
      return {
        plugins: [
          // reduce total size of the app
          new webpack.optimize.LimitChunkCountPlugin({
            maxChunks: 1,
          }),
          // use only pre compressed files
          new CompressionPlugin({
            deleteOriginalAssets: true,
          }),
        ],
      }
    } else {
      // mutate for development...
    }
  },
}
