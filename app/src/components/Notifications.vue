<template>
  <v-snackbar
    v-model="snackbar"
    timeout="-1"
  >
    {{ notifications }}

    <!--
    <template v-slot:action="{ attrs }">
      <v-btn
        color="pink"
        text
        v-bind="attrs"
        @click="snackbar = false"
      >
        Close
      </v-btn>
    </template>
    -->
  </v-snackbar>
</template>

<script>
  import { mapState } from 'vuex'

  export default {
    data: () => ({
      snackbar: false,
      timeoutHandler: null,
    }),
    computed: {
      ...mapState(['notifications']),
    },
    watch: {
      notifications () {
        this.snackbar = true
        this.resetTimeout()
      },
    },
    methods: {
      resetTimeout () {
        clearTimeout(this.timeoutHandler)

        this.timeoutHandler = setTimeout(() => {
          this.snackbar = false
        }, 3 * 1000)
      },
    },
  }
</script>

<style scoped>

</style>
