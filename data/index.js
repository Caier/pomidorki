const ax = axios;

let spiffsList = ax.get("/listfs", {headers: {'auth': localStorage.getItem('secret')}});

(async () => { //init saved panels
    await resolveData();
    updatePanels();
    setInterval(updatePanels, 20000);
})();

async function resolveData() {
    if(spiffsList instanceof Promise)
        spiffsList = (await Promise.resolve(spiffsList)).data.split(";;").filter(Boolean);
}

function stopitemgen(filename) {
    return `<li role='menuitem' class="stop" onclick="downloadSPIFFS('${filename}'); updatePanels();">
                <span class="stopName">${filename}</span>
            </li>`
}

function downloadSPIFFS(filename) {
    window.location.href = filename;
}

async function updatePanels() {
    let state = (await ax.get('/state', {responseType: 'json'})).data.state;
    $('#pom1 .progress-bar').css('width', Math.max(Math.min((425 - state.meter1) / 225, 1), 0) * 100 + '%').text(Math.round(Math.max(Math.min((425 - state.meter1) / 225, 1), 0) * 100) + '%');
    $('#pom2 .progress-bar').css('width', Math.max(Math.min((425 - state.meter2) / 225, 1), 0) * 100 + '%').text(Math.round(Math.max(Math.min((425 - state.meter2) / 225, 1), 0) * 100) + '%');
    $('#woda .progress-bar').css('width', (Math.max((21.5 - state.waterSurface), 0) / 19 * 100) + '%').text(Math.round((Math.max((21.5 - state.waterSurface), 0) / 19 * 100)) + '%');
}

function shortenPanels() {
    let hidden = $('#panels > .panel > .panel-content').filter((i, delays) => $(delays).css('display') == 'none').length;
    if(hidden < $('#panels > .panel > .panel-content').length - hidden)
        $('#panels > .panel > .panel-content').each((i, panel) => $(panel).slideUp());
    else
        $('#panels > .panel > .panel-content').each((i, panel) => $(panel).slideDown());
}

async function waterPlantsDialog() {
    let state = (await ax.get('/state', {responseType: 'json'})).data.state;
    $('#waterDialog label span').text(Math.round(state.pumpLength / 1000) + ' sekund');
    $('#waterDialog input').val(Math.round(state.pumpLength / 1000) * 1000);
}

function waterPlants() {
    if(!localStorage.getItem('secret'))
        localStorage.setItem('secret', prompt("Podaj hasło do pompy"));
    //ax.post('/pump', { length: $('#waterDialog input').val(), secret: localStorage.getItem('secret') })
    //.then(() => {
        $('#podlej').text("Podlewanie...");//.removeClass('btn-success').addClass('btn-secondary');
        setTimeout(() => $('#waterDialog').modal('hide'), $('#waterDialog input').val());
        $('#podlej').text("Podlej").removeClass('btn-secondary').addClass('btn-success');
    //})
    //.catch(err => {
    //    alert(err + '\n' + err.response.message);
    //})
}

function showResBox() {
    let lastSea = '.';
    let searchInt = setInterval(async () => {
        if(!$('#seaInput').is(':focus')) {
            clearInterval(searchInt);
            return;
        }
        let search = $('#seaInput').val().toLowerCase();
        if(lastSea == search)
            return;
        lastSea = search;
        await resolveData();
        let matchStops = spiffsList.filter(stop => stop.toLowerCase().startsWith(search))
            .sort((a, b) => a.localeCompare(b));
        $('#resBox').html('');
        matchStops.forEach(stop => {
            $('#resBox').append(stopitemgen(stop));
        });
        if($('#resBox > li').length == 0)
            $('#resBox').append(`<span class="stop nohover">brak wyników</span>`);
    }, 500);
}