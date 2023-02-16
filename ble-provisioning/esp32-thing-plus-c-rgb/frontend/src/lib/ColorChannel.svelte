<script>

  const colors = {
    R: 'red',
    G: 'green',
    B: 'blue'
  }

  export let name = "R"
  export let value = 42
  export let isDirty = false

  let color = 'red'
  $: color = colors[name]


</script>

<div class="container" style="--theme-color: {color}">
    <div class="name v-center">{name}</div>
    <div class="range-container v-center">
        <input type="range" min="0" max="255" class="slider"
               bind:value
               on:change={() => isDirty = true}>
    </div>

    <div class="preview v-center">{value}</div>
</div>

<style>
    .v-center {
        display: flex;
        align-items: center;
    }

    .container {
        width: 100%;
        display: flex;
        flex-direction: row;
        padding: 8px 0;
    }

    .name {
        font-size: 1.2em;
        font-weight: bold;
    }

    .range-container {
        flex-grow: 1;
        padding: 0 16px;
    }

    .range-container input {
        width: 100%;
    }

    .preview {
        width: 32px;
        height: 32px;
        justify-content: center;
        font-size: 1.2em;
        font-weight: bold;
    }

    .slider {
        -webkit-appearance: none;
        width: 100%;
        height: 15px;
        border-radius: 5px;
        background: #d3d3d3;
        outline: none;
        -webkit-transition: .2s;
        transition: opacity .2s;
    }

    .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background-color: var(--theme-color);
        cursor: pointer;
    }

    .slider::-moz-range-thumb {
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background-color: var(--theme-color);
        cursor: pointer;
    }

</style>
