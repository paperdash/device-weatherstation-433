<template>
  <v-card flat>
    <v-simple-table
      dense
    >
      <template #default>
        <thead>
          <tr>
            <th
              class="text-left"
              style="width: 100px"
            >
              Received
            </th>
            <th class="text-left" />
            <th class="text-right">
              <v-switch
                v-model="monitorMode"
                :loading="monitorModeProcessing"
                :disabled="monitorModeProcessing"
                dense
                label="Monitor mode"
                hide-details
              />
            </th>
          </tr>
        </thead>
        <tbody>
          <tr
            v-for="(sensor, i) in sensorHistory.slice().reverse()"
            :key="i + 3000"
          >
            <td>{{ sensor.last_update.toLocaleTimeString() }}</td>
            <template v-if="sensor.kind === 'monitor'">
              <td colspan="2">
                {{ sensor }}
              </td>
            </template>
            <template v-else>
              <td>{{ sensor.temperature }}°</td>
              <td>{{ sensor.humidity }}%</td>
            </template>
          </tr>
        </tbody>
      </template>
    </v-simple-table>
  </v-card>
</template>

<script>
  import { mapState } from 'vuex'

  export default {
    name: 'Live',
    data: () => ({
      monitorModeProcessing: false,
    }),
    computed: {
      monitorMode: {
        get () {
          return this.$store.state.sensors.monitorMode
        },
        set () {
          this.monitorModeProcessing = true
          this.$store.dispatch('sensors/toggleMonitorMode').then(() => {
            this.monitorModeProcessing = false
          })
        },
      },
      ...mapState({
        sensors: state => state.sensors.list,
        sensorHistory: state => state.sensors.history,
      }),
    },
    watch: {
      monitorMode (state) {
        console.log(state)
      },
    },
    methods: {
    },
  }
</script>

<style scoped>

</style>
