<template>
  <div>
    <v-tabs
      v-model="tabs"
      background-color="orange darken-2"
      centered
      slider-color="black"
      dark
      icons-and-text
    >
      <v-tab>
        Favorite
        <v-icon>$ratingEmpty</v-icon>
      </v-tab>
      <v-tab>
        Discovered
        <svg fill="white" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 1000" height="24"><path d="M500 990c-197.8 0-384.8-65.2-384.8-145.7 0-52.6 79.9-98.7 190.4-124.3l26.1 45.7c-94.2 16.4-163.5 45.4-163.5 78.6 0 51.2 164.4 92.7 331.7 92.7 167.3 0 331.7-41.5 331.7-92.7 0-33.2-69.3-62.2-163.5-78.6l26.1-45.7c110.6 25.6 190.4 71.7 190.4 124.3C884.7 924.8 697.8 990 500 990zm255.9-445.3c-9 9.8-221.5 257-221.5 257-8.9 10.8-18.7 18.4-29.2 18.4-10.6 0-18.5-8.4-26.6-18.4 0 0-239.6-261.9-243.7-267.6-4.1-5.7-9.6-12.9-14.1-20.4-31.6-50.6-52.5-108.6-52.5-172.6C168.3 158.2 316.8 10 500 10s331.7 148.2 331.7 331.1c0 66-19.6 127.4-53.1 179-9.1 12-17.8 19.7-22.7 24.6zM500 63c-153.9 0-278.6 124.5-278.6 278.1 0 50.7 15.5 95.1 41 132.1 8.4 13.3 34.6 44.2 34.6 44.2l203 235.4L698.8 525s10.1-11.9 22.1-29.4c30.8-44.5 57.7-93.2 57.7-154.5C778.6 187.5 653.9 63 500 63zm0 397.3c-65.9 0-119.4-53.4-119.4-119.2 0-65.8 53.5-119.2 119.4-119.2s119.4 53.4 119.4 119.2c0 65.8-53.5 119.2-119.4 119.2zm0-185.4c-36.6 0-66.3 29.7-66.3 66.2 0 36.6 29.7 66.2 66.3 66.2 36.6 0 66.3-29.7 66.3-66.2 0-36.6-29.7-66.2-66.3-66.2z"></path></svg>
      </v-tab>
      <v-tab>
        Live
        <v-icon>$history</v-icon>
      </v-tab>
    </v-tabs>

    <v-tabs-items v-model="tabs">
      <v-tab-item>
        <v-layout wrap class="pt-5">
          <v-flex
            v-for="(sensor, i) in favoriteList"
            :key="i" xs12 sm6 md4 lg3
          >
            <v-card v-if="1" outlined class="text-center mt-16 mb-6 mx-6">
              <v-card-text>
                <v-responsive
                  class="mx-auto rounded-circle elevation-10 white"
                  style="position: absolute; top: -65px; left: calc(50% - 50px);"
                  height="100"
                  width="100"
                >
                  <p class="text-no-wrap text-h4 black--text mt-6 mb-0">{{ sensor.temperature }}°</p>
                  <v-divider></v-divider>
                  <p class="text-no-wrap text-subtitle-1 grey lighten-5 pb-5">{{ sensor.humidity }}%</p>
                </v-responsive>
              </v-card-text>

              <v-card-text class="text-h4">{{sensor.label }}</v-card-text>

              <v-divider></v-divider>

              <v-card-actions class="grey lighten-5 pa-3">
                <small
                  :title="sensor.last_update | moment('LLLL')"
                >{{ sensor.last_update | moment("from") }}</small>
              </v-card-actions>
            </v-card>
          </v-flex>
        </v-layout>
      </v-tab-item>
      <v-tab-item>
        <v-card flat>
          <!--<v-card-title class="headline">Discovered</v-card-title>-->

          <v-list>
            <template
              v-for="(sensor, i) in recentlyActivity"
            >
              <v-divider
                v-if="i > 0"
                :key="i"
                style="margin-left: 95px"
              ></v-divider>

              <!--
              <v-subheader
                v-if="isOutdated(sensor.last_update)"
                :key="sensor.id"
              >
                Inactive | Favorite
              </v-subheader>
              -->

              <v-list-item
                :key="i"
                :class="{'outdated': isOutdated(sensor.last_update)}"
                @click="openEditSensorDialog(sensor)"
              >
                <v-list-item-avatar height="64" width="64">
                  <v-avatar color="orange" size="64">
                    <span class="white--text text-h6">
                      {{ sensor.temperature }}°
                    </span>
                  </v-avatar>
                </v-list-item-avatar>

                <v-list-item-content>
                  <v-list-item-title>
                    {{ sensor.last_update | moment("from") }} ( {{sensor.label }} )
                  </v-list-item-title>
                  <v-list-item-subtitle>
                    {{ sensor.humidity }}%
                    | {{ sensor.id }}, {{ sensor.protocol }}
                  </v-list-item-subtitle>
                </v-list-item-content>

                <v-list-item-action>
                  <v-icon>$settings</v-icon>
                </v-list-item-action>
              </v-list-item>
            </template>
          </v-list>

          <v-dialog
            v-model="editSensorDialog"
            width="500"
          >
            <setup-sensor
              :onCommit="onCommit"

              v-bind:sensor="editSensor"
              v-bind:sensors="sensors"
              @close="editSensorModal = false"
              @done="editSensorModal = false"
            />
          </v-dialog>

        </v-card>
      </v-tab-item>
      <v-tab-item>
        <v-card flat>
          <v-simple-table>
            <template v-slot:default>
              <thead>
              <tr>
                <th class="text-left" width="150">Time</th>
                <th class="text-left" width="50"></th>
                <th class="text-left"></th>
              </tr>
              </thead>
              <tbody>
              <tr v-for="(sensor, i) in history" :key="i">
                <td>{{ sensor.timestamp | moment("LTS") }}</td>
                <td>{{ sensor.temperature }}°</td>
                <td>{{ sensor.humidity }}%</td>
              </tr>
              </tbody>
            </template>
          </v-simple-table>

          <!--
          <v-list>
            <v-list-item v-for="(sensor, i) in history" :key="i">
              <v-list-item-content>
                {{ sensor.temperature }} / {{ sensor.humidity }}% / {{ sensor.timestamp | moment("LTS") }}
              </v-list-item-content>
            </v-list-item>
          </v-list>
          -->
        </v-card>
      </v-tab-item>
    </v-tabs-items>
  </div>
</template>

<script>
  // import apiDevice from "@/api/device";
  import { mapState } from 'vuex'
  import SetupSensor from "@/components/SetupSensor";

  export default {
    components: {
      SetupSensor
    },
    data: () => ({
      tabs: null,
      history: [],

      editSensorDialog: false,
      editSensor: null
    }),
    created() {

      this.$store.dispatch('getSensors')


      // get push date from sensors
      const connection = new WebSocket("ws://" + window.location.host + "/ws");
      const self = this
      connection.onmessage = function (event) {
        let log = JSON.parse(event.data)
        log['timestamp'] = new Date()

        console.log(log)
        self.history.unshift(log)
      };
    },
    computed: {
      favoriteList () {
        // todo filter by real fav eg. defined name
        return this.sensors.filter(sensor => !this.isOutdated(sensor.last_update))
      },
      recentlyActivity () {
        return this.sensors.slice(0).sort((a, b) => (a.last_update > b.last_update) ? -1 : 1)
      },
      ...mapState(['sensors'])
    },
    methods: {
      isOutdated(time) {
        return this.$moment().diff(time * 1000, 'hours') > 1
      },
      openEditSensorDialog(sensor) {
        this.editSensorDialog = sensor
        this.editSensor = sensor
        this.dialog = true
      },

      onCommit() {

      }
    }
  };
</script>

<style scoped>
  .outdated {
    opacity: 0.5;
  }
</style>