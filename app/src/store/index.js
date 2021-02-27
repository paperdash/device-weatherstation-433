import Vue from 'vue'
import Vuex from 'vuex'
// import api from '@/api/device.js'
import axios from 'axios'

Vue.use(Vuex)

const store = new Vuex.Store({
  state: {
    sensors: [],
    pushUpdate: false,
    notifications: {},
    sensorHistory: [],
  },
  mutations: {
    setSensors (state, payload) {
      state.sensors = payload
    },
    updateSensor (state, payload) {
      // update sensor
      const i = state.sensors.findIndex(item => item.id === payload.id)
      if (i >= 0) {
        state.sensors[i].temperature = payload.temperature
        state.sensors[i].humidity = payload.humidity
        state.sensors[i].last_update = payload.last_update

        if (payload.label) {
          state.sensors[i].label = payload.label
        }

        // update state
        state.sensors = [
          ...state.sensors,
        ]
      }
    },
    deleteSensor (state, id) {
      // const i = state.sensors.findIndex(item => item.id === id)
      state.sensors = state.sensors.filter(item => item.id !== id)
    },
    setPushUpdate (state, enable) {
      state.pushUpdate = enable
    },
    addSensorHistory (state, payload) {
      state.sensorHistory.push(payload)
      if (state.sensorHistory.length > 20) {
        state.sensorHistory = state.sensorHistory.slice(1)
      }
    },
    notification (state, payload) {
      state.notifications = payload
    },
  },
  actions: {
    async getSensors ({ commit }) {
      try {
        const response = await axios.get('/api/sensors')
        commit('setSensors', response.data)
      } catch (error) {
        commit('setSensors', [])
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

    /*
    async getRecipes({ state, commit }, plan) {
      try {
        let response = await axios.get(`${state.apiUrl}`, {
          params: {
            q: plan,
            app_id: '5b6623d5',
            app_key: '46674aa2193dbb7b88ffd897331e661a',
            from: 0,
            to: 9
          }
        });
        commit('setRecipes', response.data.hits);
      } catch (error) {
        commit('setRecipes', []);
      }
    },
    userLogin({ commit }, { email, password }) {
      firebase
        .auth()
        .signInWithEmailAndPassword(email, password)
        .then(user => {
          commit('setUser', user);
          commit('setIsAuthenticated', true);
          router.push('/about');
        })
        .catch(() => {
          commit('setUser', null);
          commit('setIsAuthenticated', false);
          router.push('/');
        });
    },
    userJoin({ commit }, { email, password }) {
      firebase
        .auth()
        .createUserWithEmailAndPassword(email, password)
        .then(user => {
          commit('setUser', user);
          commit('setIsAuthenticated', true);
          router.push('/about');
        })
        .catch(() => {
          commit('setUser', null);
          commit('setIsAuthenticated', false);
          router.push('/');
        });
    },
    userSignOut({ commit }) {
      firebase
        .auth()
        .signOut()
        .then(() => {
          commit('setUser', null);
          commit('setIsAuthenticated', false);
          router.push('/');
        })
        .catch(() => {
          commit('setUser', null);
          commit('setIsAuthenticated', false);
          router.push('/');
        });
    },
    addRecipe({ state }, payload) {
      firebase
        .database()
        .ref('users')
        .child(state.user.user.uid)
        .push(payload.recipe.label);
    },
    getUserRecipes({ state, commit }) {
      return firebase
        .database()
        .ref('users/' + state.user.user.uid)
        .once('value', snapshot => {
          commit('setUserRecipes', snapshot.val());
        });
    }
    */
  },
  getters: {
    /*
    isAuthenticated(state) {
      return state.user !== null && state.user !== undefined;
    }
     */
  },
})

// sensor push data
const connection = new WebSocket('ws://' + window.location.host + '/ws')
connection.onmessage = (message) => {
  const log = JSON.parse(message.data)
  log.last_update = new Date()

  store.commit('updateSensor', log)
  store.commit('addSensorHistory', log)
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
