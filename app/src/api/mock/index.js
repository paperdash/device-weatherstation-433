import wifiScan from './data/wifiScan.json'
import wifiConnect from './data/wifiConnect.json'
import displayScan from './data/displayScan.json'
import emptyResponse from './data/emptyResponse.json'
import settings from './data/settings.json'
import stats from './data/stats.json'

const fetch = (mockData, time = 0) => {
  return new Promise((resolve) => {
    setTimeout(() => {
      resolve(mockData)
    }, time)
  })
}

export default {
  wifiScan () {
    return fetch(wifiScan, 1000)
  },

  wifiConnect (ssid, password) {
    return fetch(wifiConnect, 1000)
  },

  async displayScan () {
    return fetch(displayScan, 1000)
  },

  displayConnect (host) {
    return fetch(emptyResponse, 1000)
  },

  displayUpdate () {
    return fetch(emptyResponse, 1000)
  },

  getSettings () {
    return fetch(settings, 1000)
  },

  putSettings () {
    return fetch(emptyResponse, 1000)
  },

  getStats () {
    return fetch(stats, 1000)
  },
}
