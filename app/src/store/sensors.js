import apiDevice from 'api-device'

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
      const list = await apiDevice.getSensors()
      commit('setList', list)
    } catch (error) {
      commit('setList', [])
    }
  },
  async update ({ commit }, [id, sensor]) {
    try {
      await apiDevice.updateSensor(id, {
        label: sensor.label,
      })

      commit('update', sensor)
    } catch (error) {
      console.warn(error)
    }
  },
  async delete ({ commit }, id) {
    try {
      await apiDevice.deleteSensor(id)
      commit('delete', id)
      commit('notification', 'sensor #' + id + ' deleted', { root: true })
    } catch (error) {
      console.warn(error)
    }
  },
  async toggleMonitorMode ({ commit, state }) {
    try {
      const toggle = !state.monitorMode
      const mode = toggle ? 'on' : 'off'

      await apiDevice.updateSensorMonitorMode(mode)
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
