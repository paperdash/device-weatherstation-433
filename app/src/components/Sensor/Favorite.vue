<template>
  <v-row class="d-flex mt-2">
    <v-col
      v-for="(sensor, i) in favoriteList"
      :key="i + 1000"
    >
      <v-card
        color="grey lighten-4"
        flat
        elevation="0"
        min-width="160"
        rounded="xl"
      >
        <v-card-text class="pb-0">
          <v-avatar
            color="grey darken-1"
            size="70"
            class="subtitle-1 white--text"
          >
            <template v-if="isOffline(sensor)">
              --
            </template>
            <template v-else>
              {{ sensor.temperature }}
            </template>
            °C
          </v-avatar>
          <v-avatar
            color="grey darken-1"
            size="48"
            class="ml-5 white--text"
          >
            <template v-if="isOffline(sensor)">
              --
            </template>
            <template v-else>
              {{ sensor.humidity }}
            </template>
            %
          </v-avatar>
        </v-card-text>

        <v-card-title>
          {{ sensor.label }}
          <template v-if="isOffline(sensor)">
            <v-spacer />
            <v-icon color="red">
              $error
            </v-icon>
          </template>
        </v-card-title>
      </v-card>
    </v-col>
  </v-row>
</template>

<script>
  import { mapState, mapGetters } from 'vuex'

  export default {
    name: 'Favorite',
    computed: {
      favoriteList () {
        return this.sensors.filter(sensor => sensor.label)
      },
      ...mapState({
        sensors: state => state.sensors.list,
        sensorHistory: state => state.sensors.history,
      }),
      ...mapGetters({
        isOffline: 'sensors/isOffline',
      }),
    },
  }
</script>

<style scoped>

</style>
