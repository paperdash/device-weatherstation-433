import Vue from 'vue';
import Vuex from 'vuex';
// import api from '@/api/device.js'
import axios from 'axios';

Vue.use(Vuex);

const store = new Vuex.Store({
	state: {
		sensors: [],
		liveUpdates: false,
		//recipes: [],
		// apiUrl: 'https://api.edamam.com/search',
		//user: null,
		//isAuthenticated: false,
		//userRecipes: []
	},
	mutations: {
		setSensors(state, payload) {
			state.sensors = payload;
		},
		updateSensor(state, id, sensor) {
			// TODO
			console.log(state)
			console.log(id)
			console.log(sensor)
			/*
			state.sensors = [
				...state.sensors.filter(element => element.id !== id), sensor
			]
			 */
		},
		setLiveUpdate(state, enable) {
			state.liveUpdates = enable
		}
		/*
		setRecipes(state, payload) {
			state.recipes = payload;
		},
		setUser(state, payload) {
			state.user = payload;
		},
		setIsAuthenticated(state, payload) {
			state.isAuthenticated = payload;
		},
		setUserRecipes(state, payload) {
			state.userRecipes = payload;
		}
	 	*/
	},
	actions: {
		async getSensors({ commit }) {
			try {
				let response = await axios.get('/api/sensors')
				commit('setSensors', response.data);
			} catch (error) {
				commit('setSensors', []);
			}
		},
		async updateSensor({ commit }, [id, sensor]) {
			try {
				await axios.put('/api/sensor/' + id, {
					label: sensor.label
				})
				commit('updateSensor', id, sensor);
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
		isAuthenticated(state) {
			return state.user !== null && state.user !== undefined;
		}
	}
});

/*
const connection = new WebSocket("ws://" + window.location.host + "/ws")
connection.onmessage = (message) => {
	let sensor = JSON.parse(message.data)
	sensor['last_update'] = new Date()

	console.log(sensor)
	//store.commit('updateSensor', [sensor.id, sensor]);
}
*/


export default store