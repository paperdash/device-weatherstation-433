import Vue from 'vue'
import Vuex from 'vuex'
import app from '@/store/app'
import sensors from '@/store/sensors'

Vue.use(Vuex)

const store = new Vuex.Store({
  modules: {
    app,
    sensors,
  },
  state: {
    notifications: {},
  },
  mutations: {
    notification (state, payload) {
      state.notifications = payload
    },
  },
  actions: { },
})

// sensor push data
const connection = new WebSocket('ws://' + window.location.host + '/ws')
connection.onmessage = (message) => {
  const log = JSON.parse(message.data)
  log.last_update = new Date()

  store.commit('sensors/update', log)
  store.commit('sensors/addHistory', log)
  store.commit('notification', log)
}

store.watch(
  state => state.pushUpdate,
  (value) => {
    if (value) {
      console.info('TODO:: enable websocket')
    } else {
      console.info('TODO:: disable websocket')
    }
  },
)

export default store
