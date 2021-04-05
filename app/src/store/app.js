import apiDevice from 'api-device'
import timezones from 'countries-and-timezones'

const state = () => ({
  stats: {},
  settings: {},
})

const mutations = {
  setStats (state, payload) {
    state.stats = payload
  },
  setSettings (state, payload) {
    state.settings = payload
  },
  updateSettings (state, payload) {
    const commit = { ...state.settings }

    Object.keys(payload).forEach(group => {
      if (commit[group]) {
        commit[group] = { ...commit[group], ...payload[group] }
      }
    })

    state.settings = commit
  },
}

const actions = {
  async loadStats ({ commit }) {
    try {
      const stats = await apiDevice.getStats()
      commit('setStats', stats)
    } catch (error) {
      commit('setStats', {})
    }
  },
  async loadSettings ({ commit }) {
    try {
      const settings = await apiDevice.getSettings()
      commit('setSettings', settings)

      if (settings.sensor.monitor) {
        commit('sensors/setMonitorMode', true, { root: true })
      }
    } catch (error) {
      commit('setSettings', {})
    }
  },

  async saveSettings ({ commit, state }) {
    try {
      await apiDevice.putSettings(state.settings)

      commit('notification', 'settings saved', { root: true })
    } catch (error) {
      console.warn(error)
    }
  },
}

const getters = {
  isSettingSupported: (state) => (key) => {
    const getDescendantProp = (obj, path) => {
      const arr = path.split(/[.[]['"]?/)
      let o = obj
      while (arr.length && o) {
        o = o[arr.shift().replace(/['"]?]$/, '')]
      }
      return o
    }

    return getDescendantProp(state.settings, key) !== undefined
  },
  getAvailableLanguages () {
    const list = []

    list.push({
      code: 'EN',
      name: 'English',
      native: 'English',
    })

    return list
  },

  getAvailableCountries () {
    return Object.values(timezones.getAllCountries())
  },

  getAvailableTimezones () {
    return Object.values(timezones.getAllTimezones())
  },
  getAvailableTimezone: () => (countryCode) => {
    return timezones.getCountry(countryCode)
  },
  getTimezoneByCountry: () => (country) => {
    return timezones.getTimezonesForCountry(country)
  },
  getTimezone: () => (timezone) => {
    return timezones.getTimezone(timezone)
  },
  getFSUsage: (state) => {
    return Math.round(
      (100 / state.stats.device.fs.total) * state.stats.device.fs.used,
    )
  },
  getMemoryUsage: (state) => {
    return Math.round(
      (100 / state.stats.device.heap.total) * state.stats.device.heap.free,
    )
  },
}

export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations,
}
