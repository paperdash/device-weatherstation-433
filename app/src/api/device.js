/**
 * Mocking client-server processing
 */

// eslint-disable-next-line
import axios from 'axios'

// eslint-disable-next-line
const _settings = {
  system: {
    country: 'AT',
    language: 'de',
    timezone: '',
    utc: 0,
    dst: 0,
    wifi: '',
  },
  wifi: {
    ssid: '',
  },
  display: {
    host: 'paperdash-epd',
    theme: 'black',
  },
}

// eslint-disable-next-line
const _stats = {
  wifi: {
    ssid: 'xd-design.info',
    connected: true,
    ip: '192.168.178.62',
    mac: '30:AE:A4:21:20:40',
    channel: 1,
    dns: '192.168.178.1',
    gateway: '192.168.178.1',
  },
  device: {
    heap: 120496,
    bootCycle: 1,
    fs: {
      total: 1860161,
      used: 1107663,
      free: 752498,
    },
  },
  sensors: [
    {
      id: 2245,
      temperature: 21.9,
      humanity: 80,
    },
    {
      id: 40,
      temperature: 23.5,
      humanity: 64,
    },
  ],
}

// eslint-disable-next-line
const _wifiScan = [{
  rssi: -59,
  ssid: 'paperdash.io',
  bssid: '38:10:D5:34:80:1B',
  channel: 11,
  secure: 3,
}, { rssi: -75, ssid: 'FRITZ!Box 7430 JI', bssid: '38:10:D5:5D:FE:7C', channel: 1, secure: 3 }, {
  rssi: -87,
  ssid: 'Vodafone Hotspot',
  bssid: 'AA:0E:14:BD:50:ED',
  channel: 1,
  secure: 0,
}, { rssi: -88, ssid: 'WLAN-548426', bssid: 'E0:60:66:55:7F:C5', channel: 1, secure: 3 }, {
  rssi: -89,
  ssid: 'Familie Kalinowski',
  bssid: 'C8:0E:14:BD:50:ED',
  channel: 1,
  secure: 3,
}, { rssi: -91, ssid: 'WLAN-507287', bssid: 'E0:60:66:48:6C:6B', channel: 1, secure: 3 }, {
  rssi: -94,
  ssid: 'TP-LINK_7238',
  bssid: 'A4:2B:B0:D8:72:38',
  channel: 3,
  secure: 3,
}]

// eslint-disable-next-line
const _sensors = [{"id": 13, "temperature": 24.50, "humidity": 53, "last_update": 1596732684}, {
  id: 186,
  temperature: 24.90,
  humidity: 54,
  last_update: 1596732697,
}, { id: 161, temperature: 24.30, humidity: 53, last_update: 1596732700 }, {
  id: 71,
  temperature: 24.70,
  humidity: 52,
  last_update: 1596732710,
}]

// eslint-disable-next-line
const _epd = [{"host": "paperdash-display", "ip": "192.168.178.65", "port": 80}]

export default {

  /**
   * @param cb
   * @returns {PromiseLike<any> | Promise<any>}
   */
  getSettings (cb) {
    // return cb(_settings);

    // eslint-disable-next-line
    return axios
      .get('/api/settings')
      .then(response => cb(response.data))
  },

  /**
   * @returns {IDBRequest<IDBValidKey> | Promise<void>}
   */
  putSettings (settings, cb) {
    return axios
      .put('/api/settings', settings, {
        headers: {
          'Content-Type': 'application/json',
        },
      })
      .then(response => cb(response.data))
  },

  /**
   * scan for wifi in range
   * @param {*} cb
   */
  wifiScan (cb) {
    return axios
      .get('/api/wifi/scan')
      .then(response => cb(response.data))
  },

  /**
   * connect to wifi
   * @param {*} ssid
   * @param {*} password
   * @param {*} cb
   */
  wifiConnect (ssid, password, cb) {
    return axios
      .post('/api/wifi/connect', {
        ssid: ssid,
        password: password,
      }, {
        headers: {
          'Content-Type': 'application/json',
        },
      })
      .then(response => cb(response.data))
  },

  /**
   * scan for display in range
   */
  async displayScan () {
    const response = await axios.get('/api/device/scan')
    const found = response.data.filter(item => item.type === 'epd')

    // load additional data
    const list = []
    found.forEach(device => {
      list.push(axios.get('http://' + device.ip + ':/stats'))
    })

    const result = []
    await axios.all(list)
      .then(
        axios.spread((...responses) => {
          responses.forEach(item => {
            console.log(item)

            result.push({
              ip: item.data.wifi.ip,
              hostname: item.data.device.hostname,
              name: item.data.device.name,
            })
          })
        }),
      )

    return result
  },

  /**
   * connect to display
   * @param {*} host
   * @param {*} cb
   */
  displayConnect (host, cb) {
    return axios
      .post('/api/epd/connect', {
        host: host,
      }, {
        headers: {
          'Content-Type': 'application/json',
        },
      })
      .then(response => cb(response.data))
  },

  /**
   * trigger display update
   * @param {*} cb
   */
  displayUpdate (cb) {
    return axios
      .get('/api/epd/update')
      .then(response => cb(response.data))
  },

  /**
   * @param cb
   * @returns {PromiseLike<any> | Promise<any>}
   */
  getStats (cb) {
    // return cb(_stats);

    // eslint-disable-next-line
    return axios
      .get('/stats')
      .then(response => cb(response.data))
  },

  /**
   * @param cb
   * @returns {PromiseLike<any> | Promise<any>}
   */
  getSensors (cb) {
    // return cb(_sensors);

    // eslint-disable-next-line
    return axios
      .get('/api/sensors')
      .then(response => cb(response.data))
  },
}
