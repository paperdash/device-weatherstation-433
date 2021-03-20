import axios from 'axios'

export default {
  wifiScan () {
    return axios
      .get('/api/wifi/scan')
      .then(response => response.data)
  },

  wifiConnect (ssid, password) {
    return axios
      .post('/api/wifi/connect', {
        ssid: ssid,
        password: password,
      }, {
        headers: {
          'Content-Type': 'application/json',
        },
      })
      .then(response => response.data)
  },

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
            result.push({
              ip: item.data.wifi.ip,
              hostname: item.data.device.hostname,
              name: item.data.device.name,
              screen: item.data.device.screen,
            })
          })
        }),
      )

    return result
  },

  displayUpdate (hostname) {
    const query = hostname ? '?hostname=' + hostname : ''
    return axios
      .get('/api/display/update' + query)
      .then(response => response.data)
  },

  getSettings () {
    return axios
      .get('/api/settings')
      .then(response => response.data)
  },

  putSettings (settings) {
    return axios.put('/api/settings', settings, {
      headers: {
        'Content-Type': 'application/json',
      },
    })
  },

  getStats () {
    return axios
      .get('/stats')
      .then(response => response.data)
  },
}
