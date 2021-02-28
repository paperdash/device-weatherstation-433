import axios from 'axios'

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
      const response = await axios.get('/stats')
      commit('setStats', response.data)
    } catch (error) {
      commit('setStats', {})
    }
  },
  async loadSettings ({ commit }) {
    try {
      const response = await axios.get('/api/settings')
      commit('setSettings', response.data)
    } catch (error) {
      commit('setSettings', {})
    }
  },
  async saveSettings ({ commit, state }) {
    try {
      await axios.put('/api/settings', state.settings, {
        headers: {
          'Content-Type': 'application/json',
        },
      })
      commit('notification', 'settings saved')
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
