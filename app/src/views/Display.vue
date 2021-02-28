<template>
  <v-container fluid>
    <v-card
      flat
      rounded="lg"
    >
      <v-card-title class="display-2 mb-12 justify-center text-center">
        Connect a display
      </v-card-title>

      <v-item-group v-model="selected">
        <v-row>
          <v-col
            v-for="(display, i) in displayAvailable"
            :key="i"
            sm="6"
          >
            <v-item
              #default="{active, toggle}"
              :value="display.ip"
            >
              <div>
                <device-simulator
                  class="mx-10 mb-3"
                  frame="black"
                  front="black"
                />
                <v-card
                  outlined
                  :class="['group', {active: active}]"
                  @click="toggle"
                >
                  <v-card-title>
                    {{ display.name || display.hostname }}
                  </v-card-title>
                  <v-card-text>
                    {{ display.ip }}
                  </v-card-text>
                  <v-card-actions>
                    <v-spacer />
                    <v-btn
                      :disabled="display.isTesting"
                      text
                      color="orange lighten-2"
                      @click="onDisplayTest(display)"
                    >
                      <span v-show="!display.isTesting">Test</span>

                      <v-progress-circular
                        v-show="display.isTesting"
                        :value="display.updateProgress"
                      />
                    </v-btn>
                  </v-card-actions>
                </v-card>
              </div>
            </v-item>
          </v-col>
        </v-row>
      </v-item-group>
    </v-card>

    <v-row
      v-if="0"
      no-gutters
      justify="center"
    >
      <v-col
        lg="5"
        md="6"
        sm="8"
      >
        <v-card flat>
          <v-card-title class="display-2 mb-12 justify-center text-center">
            Choose a Display
          </v-card-title>

          <v-skeleton-loader
            v-if="isLoading"
            type="list-item-two-line,list-item-two-line,list-item-two-line"
            class="mx-auto"
          />

          <v-list v-else>
            <template v-for="(display, i) in displayAvailable">
              <div :key="i">
                <v-divider v-if="i > 0" />

                <v-list-item
                  class="px-1"
                  @click="onDisplayConnect(display.host)"
                >
                  <!--
                  <v-list-item-icon class="mr-2 ml-2">
                    <v-icon v-if="display.host === settings.display.host">$check</v-icon>
                  </v-list-item-icon>
                  -->

                  <v-list-item-content dark>
                    <v-list-item-title v-text="display.host" />
                    <v-list-item-subtitle v-text="display.ip" />
                  </v-list-item-content>

                  <v-list-item-icon>
                    <v-icon v-if="display.host === settings.display.host">
                      $check
                    </v-icon>

                    <v-icon
                      v-else-if="display.host !== connectingHost"
                      class="ml-3"
                    >
                      $next
                    </v-icon>

                    <v-progress-circular
                      v-else-if="display.host === connectingHost"
                      :size="24"
                      :width="2"
                      color="grey "
                      indeterminate
                    />
                  </v-list-item-icon>
                </v-list-item>
              </div>
            </template>

            <v-divider />

            <v-card-actions class="pa-0">
              <v-btn
                text
                color="primary"
                class="my-2"
              >
                Choose Another Display
              </v-btn>
              <v-spacer />
              <v-btn
                text
                outlined
                class="my-2"
                :disabled="isTesting"
                @click="onDisplayTest()"
              >
                <span v-show="!isTesting">Test</span>

                <v-progress-circular
                  v-show="isTesting"
                  :value="updateProgress"
                />
              </v-btn>
            </v-card-actions>
          </v-list>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
  import apiDevice from '@/api/device'
  import { mapState } from 'vuex'
  import DeviceSimulator from '@/components/DeviceSimulator'

  export default {
    name: 'Display',
    components: { DeviceSimulator },
    data: () => ({
      isLoading: true,
      selected: null,
      connectingHost: '',

      displayAvailable: [
        { ip: '192.168.178.27', host: 'my_hostname', name: 'esp lit', isTesting: false, updateProgress: 0 },
        { ip: '192.168.178.88', host: 'my_hostname', name: 'timi', isTesting: false, updateProgress: 0 },
      ],
    }),
    computed: {
      ...mapState({
        stats: state => state.app.stats,
        settings: state => state.app.settings,
      }),
    },
    created () {
      // this.isLoading = true

      /*
      apiDevice.displayScan().then(list => {
        this.displayAvailable = []

        list.forEach(device => {
          this.displayAvailable.push({
            ip: device.ip,
            hostname: device.hostname,
            name: device.name,

            isTesting: false,
            updateProgress: 0,
          })
        })

        this.isLoading = false
      })
      */
    },
    methods: {
      onDisplayConnect (host) {
        this.connectingHost = host
        apiDevice.displayConnect(host, () => {
          this.settings.display.host = host
          this.connectingHost = ''
        })
      },

      onDisplayTest (display) {
        display.isTesting = true
        display.updateProgress = 12.5

        const timer = window.setInterval(() => {
          display.updateProgress += 12.5
        }, 1000)

        window.setTimeout(() => {
          display.isTesting = false

          window.clearInterval(timer)
        }, 8000)
      },
    },
  }
</script>

<style scoped>
.v-card.group {
  padding: 1px;
}
.v-card.group.active {
  padding: 0;
  border-color: #1867c0;
  border-width: 2px;
}
.v-card.group.active:focus {
  border-color: #1867c0;
  border-width: 2px;
}
.v-card.group.active:focus:before {
  opacity: 0;
}
</style>
