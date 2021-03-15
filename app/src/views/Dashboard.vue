<template>
  <div class="pb-5">
    <v-tabs
      v-model="tabs"
      background-color="orange darken-2"
      centered
      slider-color="black"
      dark
      icons-and-text
      class="rounded-t-lg"
    >
      <v-tab>
        Favorite
        <v-icon>$ratingEmpty</v-icon>
      </v-tab>
      <v-tab>
        Discovered
        <svg
          fill="white"
          xmlns="http://www.w3.org/2000/svg"
          viewBox="0 0 1000 1000"
          height="24"
        ><path d="M500 990c-197.8 0-384.8-65.2-384.8-145.7 0-52.6 79.9-98.7 190.4-124.3l26.1 45.7c-94.2 16.4-163.5 45.4-163.5 78.6 0 51.2 164.4 92.7 331.7 92.7 167.3 0 331.7-41.5 331.7-92.7 0-33.2-69.3-62.2-163.5-78.6l26.1-45.7c110.6 25.6 190.4 71.7 190.4 124.3C884.7 924.8 697.8 990 500 990zm255.9-445.3c-9 9.8-221.5 257-221.5 257-8.9 10.8-18.7 18.4-29.2 18.4-10.6 0-18.5-8.4-26.6-18.4 0 0-239.6-261.9-243.7-267.6-4.1-5.7-9.6-12.9-14.1-20.4-31.6-50.6-52.5-108.6-52.5-172.6C168.3 158.2 316.8 10 500 10s331.7 148.2 331.7 331.1c0 66-19.6 127.4-53.1 179-9.1 12-17.8 19.7-22.7 24.6zM500 63c-153.9 0-278.6 124.5-278.6 278.1 0 50.7 15.5 95.1 41 132.1 8.4 13.3 34.6 44.2 34.6 44.2l203 235.4L698.8 525s10.1-11.9 22.1-29.4c30.8-44.5 57.7-93.2 57.7-154.5C778.6 187.5 653.9 63 500 63zm0 397.3c-65.9 0-119.4-53.4-119.4-119.2 0-65.8 53.5-119.2 119.4-119.2s119.4 53.4 119.4 119.2c0 65.8-53.5 119.2-119.4 119.2zm0-185.4c-36.6 0-66.3 29.7-66.3 66.2 0 36.6 29.7 66.2 66.3 66.2 36.6 0 66.3-29.7 66.3-66.2 0-36.6-29.7-66.2-66.3-66.2z" /></svg>
      </v-tab>
      <v-tab>
        Live
        <v-icon>$mdiPulse</v-icon>
      </v-tab>
    </v-tabs>

    <v-tabs-items v-model="tabs">
      <v-tab-item class="px-5">
        <favorite-sensors />
      </v-tab-item>
      <v-tab-item>
        <discovered-sensors />
      </v-tab-item>
      <v-tab-item>
        <live-sensors />
      </v-tab-item>
    </v-tabs-items>
  </div>
</template>

<script>
  import FavoriteSensors from '@/components/Sensor/Favorite'
  import DiscoveredSensors from '@/components/Sensor/Discovered'
  import LiveSensors from '@/components/Sensor/Live'

  export default {
    components: {
      LiveSensors,
      FavoriteSensors,
      DiscoveredSensors,
    },
    data: () => ({
      tabs: null,
    }),
    created () {
      this.$store.dispatch('sensors/load')
    },
  }
</script>

<style scoped>
  .slide-fade-enter-active {
    transition: all .3s ease;
  }
  .slide-fade-leave-active {
    transition: all .8s cubic-bezier(1.0, 0.5, 0.8, 1.0);
  }
  .slide-fade-enter, .slide-fade-leave-to {
    transform: translateX(10px);
    opacity: 0;
  }
</style>
