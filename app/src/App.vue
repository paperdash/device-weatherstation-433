<template>
  <v-app>
    <template v-if="isLoading">
      <v-overlay :absolute="true" :value="true">
        <v-progress-circular indeterminate size="64"></v-progress-circular>
      </v-overlay>
    </template>

    <template v-else>
      <v-system-bar app dark color="primary" _window _lights-out>
        <span>paperdash.io</span>
        <v-spacer></v-spacer>
        <span>weather station</span>
        <v-spacer></v-spacer>
        <template v-if="stats.wifi.connected">
          <v-icon>{{ stats.wifi.rssi | wifiIcon(0) }}</v-icon>
          <span>{{ stats.device.time | moment("LT") }}</span>
        </template>
        <template v-else>
          <v-btn to="/setup/wifi" icon>
            <v-icon color="red">$signalWifiOff</v-icon>
          </v-btn>
        </template>
      </v-system-bar>

      <v-main class="grey lighten-4">
        <v-container fluid fill-height class="align-start">
          <transition-page>
            <router-view></router-view>
          </transition-page>
        </v-container>
      </v-main>
    </template>
  </v-app>
</template>

<script>
import apiDevice from "./api/device";
import "@/assets/app.css";
import transitionPage from "@/components/TransitionPage";

export default {
  components: {
    transitionPage,
  },
  data: () => ({
    isLoading: true,
    settings: null,
  }),
  created() {
    apiDevice.getSettings((settings) => {
      this.settings = settings;

      this.autoReloadStats();
    });
  },
  watch: {
    stats() {
      this.isLoading = false;
    },
  },
  computed: {
    stats() {
      return this.$root._data.stats;
    },
  },
  methods: {
    autoReloadStats() {
      apiDevice.getStats((stats) => {
        // give esp some extra time befor fetch new data
        /*
					stats.playlist.remaining += 2

					// reset old so reactive watcher can detect a change
					if (this.$root._data.stats) {
						this.$root._data.stats.playlist.remaining = 0
					}
					*/
        this.$root._data.stats = stats;

        setTimeout(() => {
          this.autoReloadStats();
		}, 60 * 1000);
      });
    },
  },
};
</script>

<style scoped>
</style>