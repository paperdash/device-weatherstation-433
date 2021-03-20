import wifiScan from './data/wifiScan.json'
import wifiConnect from './data/wifiConnect.json'
import displayScan from './data/displayScan.json'
import emptyResponse from './data/emptyResponse.json'
import settings from './data/settings.json'
import stats from './data/stats.json'
import sensors from './data/sensors.json'

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

  displayUpdate (hostname) {
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

  getSensors () {
    // simulate activity
    const now = Date.now() / 1000 | 0

    // eslint-disable-next-line no-return-assign
    sensors.forEach((o, i, sensor) => sensor[i].last_update = i % 2 ? now : 0)

    return fetch(sensors, 1000)
  },

  deleteSensor (id) {
    return fetch(emptyResponse, 1000)
  },

  updateSensor (id, sensor) {
    return fetch(emptyResponse, 1000)
  },

  updateSensorMonitorMode (mode) {
    return fetch(emptyResponse, 1000)
  },
}
