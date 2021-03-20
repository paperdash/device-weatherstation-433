<template>
  <v-card
    flat
    rounded="lg"
    class="pa-5"
  >
    <v-card-title class="text-h3 mb-12 justify-center text-center">
      Connect a display
    </v-card-title>

    <template v-if="isLoading">
      <v-row>
        <v-col>
          <v-skeleton-loader
            type="card"
          />
        </v-col>
        <v-col>
          <v-skeleton-loader
            type="card"
          />
        </v-col>
      </v-row>
    </template>
    <template v-else>
      <v-item-group v-model="selectedIp">
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
                    <v-chip
                      label
                      small
                    >
                      <v-icon left>
                        $wifi
                      </v-icon>
                      {{ display.ip }}
                    </v-chip>
                    <br>
                    <v-chip
                      label
                      small
                    >
                      <v-icon left>
                        $mdiOverscan
                      </v-icon>
                      {{ display.screen.width }} x {{ display.screen.height }}
                    </v-chip>
                  </v-card-text>
                </v-card>

                <v-card-actions v-if="0">
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
              </div>
            </v-item>
          </v-col>
        </v-row>
      </v-item-group>

      <v-divider class="mt-12" />

      <v-card-actions>
        <v-btn
          text
          :disabled="isTesting || !selectedIp"
          @click="onDisplayTest()"
        >
          Test display
        </v-btn>
        <v-spacer />
        <v-btn
          :loading="isProcessing"
          depressed
          @click="commitChanges"
        >
          <v-icon left>
            $done
          </v-icon>
          Save
        </v-btn>
      </v-card-actions>

      <v-dialog
        v-model="isTesting"
        hide-overlay
        persistent
        width="300"
      >
        <v-card
          color="orange darken-2"
          dark
        >
          <v-card-text>
            Connecting...

            <v-progress-linear
              :value="testingProgress"
              color="white"
              class="mb-0"
            />

            <v-progress-linear
              v-if="0"
              indeterminate
              color="white"
              class="mb-0"
            />
          </v-card-text>
        </v-card>
      </v-dialog>
    </template>
  </v-card>
</template>

<script>
  import apiDevice from 'api-device'
  import { mapState, mapMutations, mapActions } from 'vuex'
  import DeviceSimulator from '@/components/DeviceSimulator'

  export default {
    name: 'Display',
    components: { DeviceSimulator },
    data: () => ({
      isLoading: true,
      isProcessing: false,
      isTesting: false,
      testingProgress: 0,
      displayAvailable: [],
    }),
    computed: {
      ...mapState({
        stats: state => state.app.stats,
        settings: state => state.app.settings,
      }),
      selectedIp: {
        get () {
          return this.settings.display.host
        },
        set (hostname) {
          this.updateSettings({
            display: {
              host: hostname,
            },
          })
        },
      },
    },
    created () {
      apiDevice.displayScan().then(list => {
        this.displayAvailable = list
        this.isLoading = false
      })
    },
    methods: {
      ...mapMutations({
        updateSettings: 'app/updateSettings',
      }),
      ...mapActions({
        saveSettings: 'app/saveSettings',
      }),

      commitChanges () {
        this.isProcessing = true

        this.saveSettings().then(() => {
          this.isProcessing = false
        })
      },

      onDisplayTest () {
        this.isTesting = true

        apiDevice.displayUpdate(this.selectedIp)

        this.testingProgress = 6.25

        const timer = window.setInterval(() => {
          this.testingProgress += 6.25
        }, 1000)

        window.setTimeout(() => {
          this.isTesting = false

          window.clearInterval(timer)
        }, 16000)
      },
    },
  }
</script>

<style scoped>
.v-card.group {
  padding: 1px;
}
.v-card--link:focus:before {
  opacity: 0;
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
