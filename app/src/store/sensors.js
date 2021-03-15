import axios from 'axios'

const state = () => ({
  list: [],
  pushUpdate: false,
  history: [],
  monitorMode: false,
})

const mutations = {
  setList (state, payload) {
    state.list = payload
  },
  update (state, payload) {
    // update sensor
    const i = state.list.findIndex(item => item.id === payload.id)
    if (i >= 0) {
      state.list[i].temperature = payload.temperature
      state.list[i].humidity = payload.humidity
      state.list[i].last_update = payload.last_update

      if (payload.label) {
        state.list[i].label = payload.label
      }

      // update state
      state.list = [
        ...state.list,
      ]
    }
  },
  delete (state, id) {
    // const i = state.list.findIndex(item => item.id === id)
    state.list = state.list.filter(item => item.id !== id)
  },
  setPushUpdate (state, enable) {
    state.pushUpdate = enable
  },
  setMonitorMode (state, enable) {
    state.monitorMode = enable
  },
  addHistory (state, payload) {
    state.history.push(payload)
    if (state.history.length > 20) {
      state.history = state.history.slice(1)
    }
  },
}

const actions = {
  async load ({ commit }) {
    try {
      const response = await axios.get('/api/sensors')
      commit('setList', response.data)
    } catch (error) {
      commit('setList', [])
    }
  },
  async putSensor ({ commit }, [id, sensor]) {
    try {
      await axios.put('/api/sensor/' + id, {
        label: sensor.label,
      })
      commit('updateSensor', sensor)
    } catch (error) {
      console.warn(error)
    }
  },
  async deleteSensor ({ commit }, id) {
    try {
      await axios.delete('/api/sensor/' + id)
      commit('deleteSensor', id)
      commit('notification', 'sensor #' + id + ' deleted')
    } catch (error) {
      console.warn(error)
    }
  },
  async toggleMonitorMode ({ commit, state }) {
    try {
      const toggle = !state.monitorMode
      const mode = toggle ? 'on' : 'off'

      await axios.get('/api/sensor/monitor?' + mode)
      commit('setMonitorMode', toggle)
      commit('notification', 'monitor mode switched ' + mode, { root: true })
    } catch (error) {
      console.warn(error)
    }
  },
}

const getters = {
  isOffline: () => (sensor) => {
    const update = new Date(sensor.last_update * 1000)
    const outdated = Date.now() - (10 * 60 * 1000)

    return update < outdated
  },
}

export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations,
}
