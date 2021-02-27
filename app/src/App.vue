<template>
  <v-app>
    <template v-if="isLoading">
      <v-overlay
        :absolute="true"
        :value="true"
      >
        <v-progress-circular
          indeterminate
          size="64"
        />
      </v-overlay>
    </template>

    <template v-else>
      <div class="fill-height grey lighten-4">
        <v-container>
          <v-card>
            <notifications />

            <v-app-bar
              color="orange darken-2"
              dark
              flat
            >
              <v-app-bar-nav-icon @click="drawer = true" />

              <v-toolbar-title>Retrofit Weather Station</v-toolbar-title>

              <v-spacer />

              <template v-if="settings.display.host">
                <v-btn
                  :href="'http://' + settings.display.host"
                  target="_blank"
                  icon
                >
                  <v-icon>$present_to_all</v-icon>
                </v-btn>
              </template>
              <template v-if="stats.wifi.connected">
                <v-icon class="mr-2">
                  {{ stats.wifi.rssi | wifiIcon(0) }}
                </v-icon>
                <v-chip
                  color="white"
                  outlined
                  pill
                >
                  {{ stats.device.time | moment("LT") }}
                </v-chip>
              </template>
              <template v-else>
                <v-btn
                  to="/setup/wifi"
                  icon
                >
                  <v-icon color="red">
                    $signalWifiOff
                  </v-icon>
                </v-btn>
              </template>
            </v-app-bar>

            <v-main
              fluid_
              _fill-height
              class="align-start"
            >
              <transition-page>
                <router-view />
              </transition-page>
            </v-main>
          </v-card>
        </v-container>

        <v-navigation-drawer
          v-model="drawer"
          absolute
          temporary
        >
          <v-list-item>
            <v-list-item-content>
              <v-list-item-title class="title">
                Application
              </v-list-item-title>
              <v-list-item-subtitle>
                subtext
              </v-list-item-subtitle>
            </v-list-item-content>
          </v-list-item>

          <v-divider class="mx-2" />

          <v-list
            nav
            dense
          >
            <v-list-item to="/">
              <v-list-item-icon>
                <v-icon>$dashboard</v-icon>
              </v-list-item-icon>
              <v-list-item-content>
                <v-list-item-title>
                  Sensor
                </v-list-item-title>
              </v-list-item-content>
            </v-list-item>
            <v-list-item to="/setup/wifi">
              <v-list-item-icon>
                <v-icon>$wifi</v-icon>
              </v-list-item-icon>
              <v-list-item-content>
                <v-list-item-title>
                  Wifi
                </v-list-item-title>
              </v-list-item-content>
            </v-list-item>
            <v-list-item to="/display">
              <v-list-item-icon>
                <v-icon>$present_to_all</v-icon>
              </v-list-item-icon>
              <v-list-item-content>
                <v-list-item-title>
                  Display
                </v-list-item-title>
              </v-list-item-content>
            </v-list-item>
            <v-list-item to="/system">
              <v-list-item-icon>
                <v-icon>$settings</v-icon>
              </v-list-item-icon>
              <v-list-item-content>
                <v-list-item-title>
                  System
                </v-list-item-title>
              </v-list-item-content>
            </v-list-item>
          </v-list>

          <template v-slot:append>
            <div class="pa-2 text-center text--disabled">
              v1.0.3
            </div>
          </template>
        </v-navigation-drawer>
      </div>
    </template>
  </v-app>
</template>

<script>
  import apiDevice from './api/device'
  import '@/assets/app.css'
  import transitionPage from '@/components/TransitionPage'
  import notifications from '@/components/Notifications'

  export default {
    components: {
      transitionPage,
      notifications,
    },
    data: () => ({
      isLoading: true,
      settings: null,
      drawer: false,
    }),
    computed: {
      stats () {
        return this.$root._data.stats
      },
    },
    watch: {
      stats () {
        this.isLoading = false
      },
    },
    created () {
      apiDevice.getSettings((settings) => {
        // TODO move to vuex
        this.settings = settings

        this.autoReloadStats()

        this.$store.commit('setPushUpdate', true)
      })
    },
    methods: {
      autoReloadStats () {
        // TODO move to vuex
        apiDevice.getStats((stats) => {
          this.$root._data.stats = stats

          setTimeout(() => {
            this.autoReloadStats()
          }, 60 * 1000)
        })
      },
    },
  }
</script>

<style scoped>
</style>
