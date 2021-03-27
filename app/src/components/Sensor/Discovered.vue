<template>
  <v-card
    flat
  >
    <v-list>
      <v-subheader>Active</v-subheader>

      <template
        v-for="(sensor, i) in activeList"
      >
        <div :key="i + 2000">
          <v-divider
            v-if="i > 0"

            style="margin-left: 95px"
          />

          <v-list-item
            @click="openEditSensorDialog(sensor)"
          >
            <v-list-item-avatar
              height="64"
              width="64"
            >
              <v-avatar
                color="orange"
                size="64"
              >
                <span class="white--text text-h6">
                  {{ sensor.temperature }}°
                </span>
              </v-avatar>
            </v-list-item-avatar>

            <v-list-item-content>
              <v-list-item-title>
                {{ sensor.last_update.toLocaleTimeString() }}
                <span v-if="sensor.label">
                  ({{ sensor.label }})
                </span>
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
        </div>
      </template>

      <v-subheader v-if="outdatedList.length > 0">
        Inactive
      </v-subheader>

      <template
        v-for="(sensor, i) in outdatedList"
      >
        <div :key="i + 3000">
          <v-divider
            v-if="i > 0"
            style="margin-left: 95px"
          />

          <v-list-item
            class="outdated"
          >
            <v-list-item-avatar
              height="64"
              width="64"
            >
              <v-avatar
                color="orange"
                size="64"
              >
                <span class="white--text text-h6">
                  {{ sensor.temperature }}°
                </span>
              </v-avatar>
            </v-list-item-avatar>

            <v-list-item-content>
              <v-list-item-title>
                <template v-if="sensor.last_update">
                  {{ sensor.last_update.toLocaleString() }}
                </template>

                <span v-if="sensor.label">
                  ({{ sensor.label }})
                </span>
              </v-list-item-title>
              <v-list-item-subtitle>
                {{ sensor.humidity }}%
                | {{ sensor.id }}, {{ sensor.protocol }}
              </v-list-item-subtitle>
            </v-list-item-content>

            <v-list-item-action>
              <v-btn
                icon
                @click="deleteSensor(sensor.id)"
              >
                <v-icon>$delete</v-icon>
              </v-btn>
            </v-list-item-action>
          </v-list-item>
        </div>
      </template>
    </v-list>

    <v-dialog
      v-model="editSensorDialog"
      width="500"
    >
      <setup-sensor
        :on-commit="onCommit"

        :sensor="editSensor"
        :sensors="sensors"
        @close="editSensorDialog = false"
        @done="editSensorDialog = false"
      />
    </v-dialog>
  </v-card>
</template>

<script>
  import { mapActions, mapGetters, mapState } from 'vuex'
  import SetupSensor from '@/components/SetupSensor'

  export default {
    name: 'Discovered',
    components: {
      SetupSensor,
    },
    data: () => ({
      editSensorDialog: false,
      editSensor: null,
    }),
    computed: {
      recentlyActivity () {
        return this.sensors.slice(0).sort((a, b) => b.last_update - a.last_update)
      },
      activeList () {
        return this.recentlyActivity.filter(sensor => !this.isOffline(sensor)).sort((a, b) => (a.id < b.id) ? -1 : 1)
      },
      outdatedList () {
        return this.recentlyActivity.filter(sensor => this.isOffline(sensor))
      },
      ...mapState({
        sensors: state => state.sensors.list,
        sensorHistory: state => state.sensors.history,
      }),
      ...mapGetters({
        isOffline: 'sensors/isOffline',
      }),

    },
    methods: {
      openEditSensorDialog (sensor) {
        this.editSensorDialog = sensor
        this.editSensor = sensor
        this.dialog = true
      },

      ...mapActions({
        deleteSensor: 'sensors/delete',
      }),

      onCommit () {

      },
    },
  }
</script>

<style scoped>
.outdated {
  opacity: 0.5;
}
</style>
