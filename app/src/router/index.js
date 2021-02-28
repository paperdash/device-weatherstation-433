import Vue from 'vue'
import VueRouter from 'vue-router'

const Dashboard = () => import('../views/Dashboard')
const Wifi = () => import('../views/Wifi')
const System = () => import('../views/System')
const Display = () => import('../views/Display')

const SetupStart = () => import('../views/Setup/Start')
// const SetupCountry = () => import('../views/Setup/Country')
// const SetupWifi = () => import('../views/Setup/Wifi')
// const SetupDone = () => import('../views/Setup/Done')

Vue.use(VueRouter)

export default new VueRouter({
  routes: [
    { path: '/', component: Dashboard },
    { path: '/display', component: Display, meta: { transitionName: 'slide' } },
    { path: '/wifi', component: Wifi, meta: { transitionName: 'slide' } },
    { path: '/system', component: System, meta: { transitionName: 'slide' } },

    // setup wizard
    { path: '/setup/start', component: SetupStart, meta: { transitionName: 'slide' } },
    // { path: '/setup/country', component: SetupCountry, meta: { transitionName: 'slide' } },
    // { path: '/setup/wifi', component: SetupWifi, meta: { transitionName: 'slide' } },
    // { path: '/setup/done', component: SetupDone, meta: { transitionName: 'slide' } },

    { path: '*', redirect: '/' },
  ],
})
