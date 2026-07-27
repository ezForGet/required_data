<template>
  <button class="btn" @click="$emit('click')">
    <slot>{{ text }}</slot>
  </button>
</template>

<script setup lang="ts">
defineProps<{
  text?: string
}>()

defineEmits<{
  click: []
}>()
</script>

<style scoped>
.btn {
  padding: 10px 20px;
  background: #3b82f6;
  color: white;
  border: none;
  border-radius: 6px;
  cursor: pointer;
  font-size: 14px;
}
.btn:hover {
  background: #2563eb;
}
</style>