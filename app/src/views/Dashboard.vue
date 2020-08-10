<template>
  <v-container _fluid _grid-list-md _pa-2>
    <div wrap_>
      <v-card>
        <v-tabs
          v-model="tabs"
          background-color="orange darken-2"
          centered
          dark
          icons-and-text
        >
          <v-tab>
            Favorite
            <svg fill="white" height="24" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 243.317 243.317"><path d="M242.949 93.714a7.498 7.498 0 00-6.054-5.104l-74.98-10.9-33.53-67.941a7.501 7.501 0 00-13.451 0L81.404 77.71 6.422 88.61a7.501 7.501 0 00-4.157 12.792l54.257 52.886-12.807 74.672a7.5 7.5 0 0010.881 7.906l67.064-35.254 67.063 35.254a7.49 7.49 0 003.489.861H192.234a7.5 7.5 0 007.277-9.324l-12.713-74.117 54.254-52.885a7.496 7.496 0 001.897-7.687zm-69.445 52.585a7.498 7.498 0 00-2.157 6.639l10.906 63.581-57.102-30.018a7.496 7.496 0 00-6.979 0l-57.103 30.018 10.906-63.581a7.499 7.499 0 00-2.157-6.639l-46.199-45.031 63.847-9.281a7.498 7.498 0 005.647-4.103l28.55-57.849 28.55 57.849a7.5 7.5 0 005.646 4.103l63.844 9.281-46.199 45.031z"/></svg>
          </v-tab>
          <v-tab>
            Discovered
            <svg fill="white" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 1000" height="24"><path d="M500 990c-197.8 0-384.8-65.2-384.8-145.7 0-52.6 79.9-98.7 190.4-124.3l26.1 45.7c-94.2 16.4-163.5 45.4-163.5 78.6 0 51.2 164.4 92.7 331.7 92.7 167.3 0 331.7-41.5 331.7-92.7 0-33.2-69.3-62.2-163.5-78.6l26.1-45.7c110.6 25.6 190.4 71.7 190.4 124.3C884.7 924.8 697.8 990 500 990zm255.9-445.3c-9 9.8-221.5 257-221.5 257-8.9 10.8-18.7 18.4-29.2 18.4-10.6 0-18.5-8.4-26.6-18.4 0 0-239.6-261.9-243.7-267.6-4.1-5.7-9.6-12.9-14.1-20.4-31.6-50.6-52.5-108.6-52.5-172.6C168.3 158.2 316.8 10 500 10s331.7 148.2 331.7 331.1c0 66-19.6 127.4-53.1 179-9.1 12-17.8 19.7-22.7 24.6zM500 63c-153.9 0-278.6 124.5-278.6 278.1 0 50.7 15.5 95.1 41 132.1 8.4 13.3 34.6 44.2 34.6 44.2l203 235.4L698.8 525s10.1-11.9 22.1-29.4c30.8-44.5 57.7-93.2 57.7-154.5C778.6 187.5 653.9 63 500 63zm0 397.3c-65.9 0-119.4-53.4-119.4-119.2 0-65.8 53.5-119.2 119.4-119.2s119.4 53.4 119.4 119.2c0 65.8-53.5 119.2-119.4 119.2zm0-185.4c-36.6 0-66.3 29.7-66.3 66.2 0 36.6 29.7 66.2 66.3 66.2 36.6 0 66.3-29.7 66.3-66.2 0-36.6-29.7-66.2-66.3-66.2z"></path></svg>
          </v-tab>
        </v-tabs>

        <v-tabs-items v-model="tabs">
          <v-tab-item>
            <v-layout wrap class="mt-5">
              <v-flex
                v-for="sensor in favoriteList"
                :key="sensor.id" xs12 sm6 md4 lg3
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

                  <v-card-text class="text-h4 font-italic">Name</v-card-text>

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
                    :key="sensor.id"
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
                    :key="sensor.id"
                    :class="{'outdated': isOutdated(sensor.last_update)}"
                    @click="addFavorite"
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
                        {{ sensor.last_update | moment("from") }}
                      </v-list-item-title>
                      <v-list-item-subtitle>
                        {{ sensor.humidity }}%
                        | protocol: wt_800
                        | device id: 163
                      </v-list-item-subtitle>
                    </v-list-item-content>

                    <v-list-item-action>
                      <v-btn icon>
                        <svg fill="black" width="24" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 489.8 489.8"><path d="M438.2 0H51.6C23.1 0 0 23.2 0 51.6v386.6c0 28.5 23.2 51.6 51.6 51.6h386.6c28.5 0 51.6-23.2 51.6-51.6V51.6c0-28.4-23.2-51.6-51.6-51.6zm27.1 438.2c0 14.9-12.2 27.1-27.1 27.1H51.6c-14.9 0-27.1-12.2-27.1-27.1V51.6c0-14.9 12.2-27.1 27.1-27.1h386.6c14.9 0 27.1 12.2 27.1 27.1v386.6z"/><path d="M337.4 232.7h-80.3v-80.3c0-6.8-5.5-12.3-12.3-12.3s-12.3 5.5-12.3 12.3v80.3h-80.3c-6.8 0-12.3 5.5-12.3 12.2 0 6.8 5.5 12.3 12.3 12.3h80.3v80.3c0 6.8 5.5 12.3 12.3 12.3s12.3-5.5 12.3-12.3v-80.3h80.3c6.8 0 12.3-5.5 12.3-12.3 0-6.8-5.5-12.2-12.3-12.2z"/></svg>
                      </v-btn>
                    </v-list-item-action>
                  </v-list-item>
                </template>
              </v-list>
            </v-card>
          </v-tab-item>
        </v-tabs-items>
      </v-card>
    </div>
  </v-container>
</template>

<script>
  import apiDevice from "@/api/device";

  export default {
    data: () => ({
      sensors: [],
      connection: null,
      tabs: null
    }),
    created() {
      apiDevice.getSensors((list) => {
        //this.sensors = list;
        // sort by recently activity
        this.sensors = list.sort((a, b) => (a.last_update > b.last_update) ? -1 : 1);
      });

      console.log("Starting connection to WebSocket Server");
      this.connection = new WebSocket("ws://" + window.location.host + "/ws");

      this.connection.onmessage = function (event) {
        // TODO read sensor json data
        console.log(event.data);
      };

      /*
      this.connection.onopen = function(event) {
        //console.log(event)
        //console.log("Successfully connected to the echo websocket server...")
      }
      */
    },
    computed: {
      favoriteList () {
        // todo filter by real fav eg. defined name
        return this.sensors.filter(sensor => !this.isOutdated(sensor.last_update))
      },
      recentlyActivity () {
        return this.sensors.slice(0).sort((a, b) => (a.last_update > b.last_update) ? -1 : 1)
      }
    },
    methods: {
      isOutdated(time) {
        return this.$moment().diff(time * 1000, 'hours') > 1
      },
      addFavorite() {
        alert()
      }
    }
  };
</script>

<style scoped>
  .outdated {
    opacity: 0.5;
  }
</style>