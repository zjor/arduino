<script>
  import {onMount} from 'svelte'
  import ColorChannel from './lib/ColorChannel.svelte'
  import {apiClient} from "./lib/client"

  let r = 0
  let g = 0
  let b = 0

  let rDirty, gDirty, bDirty;

  let color
  $: color = `rgb(${r}, ${g}, ${b})`

  async function applyColor() {
    const newState = await apiClient.setRGBState({r, g, b})
    r = newState.r
    g = newState.g
    b = newState.b
    rDirty = false
    gDirty = false
    bDirty = false
  }

  onMount(async () => {
    const newState = await apiClient.getRGBState()
    r = newState.r
    g = newState.g
    b = newState.b
  })

</script>

<main style="--theme-color: {color}">
    <h1>ESP32 :: RGB LED Controller</h1>
    <div id="preview"></div>
    <div id="controls">
        <ColorChannel name="R" bind:value={r} bind:isDirty={rDirty}/>
        <ColorChannel name="G" bind:value={g} bind:isDirty={gDirty}/>
        <ColorChannel name="B" bind:value={b} bind:isDirty={bDirty}/>
    </div>
    <div class="button-container">
        <button
                disabled={!(rDirty || gDirty || bDirty)}
                on:click={applyColor}>
            Apply
        </button>
    </div>
</main>

<style>
    #preview {
        width: 100%;
        height: 96px;
        background-color: var(--theme-color);
    }

    #controls {
        padding: 2em 1em;
    }

    .button-container {
        width: 100%;
        display: flex;
        flex-direction: row;
        justify-content: center;
    }

    button {
        width: 150px;
        height: 50px;
    }

</style>
